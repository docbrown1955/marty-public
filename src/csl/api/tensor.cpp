// This file is part of MARTY.
//
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

/**
 * @file tensor.cpp
 * @brief Implementation of the Python neutral handles on spaces, indices and
 * tensors.
 */

#include "tensor.h"

#include <algorithm>
#include <cstdlib>
#include <set>
#include <string_view>

namespace csl::api {

namespace {

/* Rejects symmetry declarations that are invalid or that CSL would silently
 * drop: CSL ignores addSymmetry() after setFullySymmetric(), and
 * addAntiSymmetry() drops full symmetry. Only direct conflicts are detected
 * here. Combinations that are only inconsistent through a group closure are
 * not detected. */
void checkSymmetries(std::size_t         rank,
                     SymmetrySpec const &symmetric,
                     SymmetrySpec const &antisymmetric)
{
    auto checkPairs = [rank](SymmetrySpec const &spec) {
        for (auto const &[first, second] : spec.pairs) {
            if (first >= rank or second >= rank)
                throw ValueError("tensor symmetry: position out of range");
            if (first == second)
                throw ValueError("tensor symmetry: a pair needs two distinct "
                                 "positions");
        }
    };
    checkPairs(symmetric);
    checkPairs(antisymmetric);

    if ((symmetric.fully and not symmetric.pairs.empty())
        or (antisymmetric.fully and not antisymmetric.pairs.empty()))
        throw ValueError("tensor symmetry: give either full symmetry or "
                         "position pairs, not both");

    if ((symmetric.fully
         and (antisymmetric.fully or not antisymmetric.pairs.empty()))
        or (antisymmetric.fully and not symmetric.pairs.empty()))
        throw ValueError("tensor symmetry: conflicting symmetric and "
                         "antisymmetric declarations");

    for (auto const &[s1, s2] : symmetric.pairs)
        for (auto const &[a1, a2] : antisymmetric.pairs)
            if ((s1 == a1 and s2 == a2) or (s1 == a2 and s2 == a1))
                throw ValueError("tensor symmetry: the same pair is declared "
                                 "both symmetric and antisymmetric");
}

csl::Tensor makeTensor(std::string const              &name,
                       std::vector<SpaceHandle> const &spaces,
                       SymmetrySpec const             &symmetric,
                       SymmetrySpec const             &antisymmetric,
                       csl::ComplexProperty            complexProperty)
{
    if (spaces.empty())
        throw ValueError("tensor " + name
                         + ": at least one index space is required");
    checkSymmetries(spaces.size(), symmetric, antisymmetric);

    std::vector<csl::Space const *> raw;
    raw.reserve(spaces.size());
    for (auto const &space : spaces)
        raw.push_back(&space.raw());

    return detail::guarded("tensor creation", [&] {
        csl::Tensor tensor(name, raw);
        if (symmetric.fully)
            tensor->setFullySymmetric();
        else
            for (auto const &[first, second] : symmetric.pairs)
                tensor->addSymmetry(static_cast<int>(first),
                                    static_cast<int>(second));
        if (antisymmetric.fully)
            tensor->setFullyAntiSymmetric();
        else
            for (auto const &[first, second] : antisymmetric.pairs)
                tensor->addAntiSymmetry(static_cast<int>(first),
                                        static_cast<int>(second));
        tensor->setComplexProperty(complexProperty);
        return tensor;
    });
}

/* Field of a point space and of index spaces. */
csl::TensorField makeField(std::string const              &name,
                           SpaceHandle const              &pointSpace,
                           std::vector<SpaceHandle> const &spaces)
{
    std::vector<csl::Space const *> raw;
    raw.reserve(spaces.size());
    for (auto const &space : spaces)
        raw.push_back(&space.raw());

    return detail::guarded("tensor field creation", [&] {
        return csl::TensorField(name, &pointSpace.raw(), raw);
    });
}

/* Index must belong to the space \p space. */
void checkIndexSpace(IndexHandle const &index,
                     csl::Space const  *space,
                     std::string_view   what)
{
    if (index.raw().getSpace() != space)
        throw ValueError(std::string(what)
                         + ": index belongs to another space");
}

/* Point must be a rank-1 tensor of the space \p space. */
void checkPoint(TensorHandle const &point,
                csl::Space const   *space,
                std::string_view    what)
{
    if (point.rank() != 1
        or point.raw()->getSpace() != std::vector<csl::Space const *>{space})
        throw ValueError(std::string(what)
                         + ": point must be a rank-1 tensor of the point "
                           "space");
}

/* Space shared by all indices. Raises ValueError if they differ or if the
 * indices have no space. */
csl::Space const *commonSpace(std::vector<IndexHandle> const &indices,
                              std::string_view                what)
{
    csl::Space const *space = indices.front().raw().getSpace();
    if (space == nullptr)
        throw ValueError(std::string(what) + ": index has no space");
    for (auto const &index : indices)
        if (index.raw().getSpace() != space)
            throw ValueError(std::string(what)
                             + ": indices belong to different spaces");
    return space;
}

/* The CSL identity of an index is relative to its space: an index without a
 * space has none to inspect. */
void requireSpace(csl::Index const &index, std::string_view what)
{
    if (index.getSpace() == nullptr)
        throw ValueError(std::string(what) + ": index has no space");
}

/* Key under which CSL identifies an index in a renaming. */
csl::IndexRenaming::Key renamingKey(csl::Index const &index)
{
    return csl::IndexRenaming::Key{
        index.getSpace(), index.getID(), std::string(index.getName())};
}

/* Distinct indices of the given type, in order of first occurrence. */
std::vector<IndexHandle>
distinctIndices(std::vector<csl::Index> const &indices, cslIndex::Type type)
{
    std::vector<IndexHandle> result;
    for (auto const &index : indices) {
        if (index.getType() != type)
            continue;
        IndexHandle handle(nullptr, index);
        bool const  seen = std::any_of(
            result.begin(), result.end(), [&](IndexHandle const &known) {
                return known.equals(handle);
            });
        if (not seen)
            result.push_back(handle);
    }
    return result;
}

} // namespace

// SpaceHandle

SpaceHandle::SpaceHandle(std::shared_ptr<csl::Space const> space)
    : m_space(std::move(space))
{
}

SpaceHandle::SpaceHandle(std::string const              &name,
                         int                             dimension,
                         std::vector<std::string> const &indexNames)
    : m_space(detail::guarded("space creation", [&] {
          if (dimension < 1)
              throw ValueError("space " + name
                               + ": dimension must be at least 1");
          return std::shared_ptr<csl::Space const>(
              std::make_shared<csl::Space>(name, dimension, indexNames));
      }))
{
}

SpaceHandle SpaceHandle::minkowski()
{
    return SpaceHandle(std::shared_ptr<csl::Space const>(
        &csl::Minkowski, [](csl::Space const *) {}));
}

SpaceHandle SpaceHandle::euclid2()
{
    return SpaceHandle(std::shared_ptr<csl::Space const>(
        &csl::Euclid_R2, [](csl::Space const *) {}));
}

SpaceHandle SpaceHandle::euclid3()
{
    return SpaceHandle(std::shared_ptr<csl::Space const>(
        &csl::Euclid_R3, [](csl::Space const *) {}));
}

SpaceHandle SpaceHandle::euclid4()
{
    return SpaceHandle(std::shared_ptr<csl::Space const>(
        &csl::Euclid_R4, [](csl::Space const *) {}));
}

int SpaceHandle::dimension() const
{
    return m_space->getDim();
}

IndexHandle SpaceHandle::index(std::string const &name) const
{
    if (name.empty())
        throw ValueError("index: name must not be empty");
    return IndexHandle(m_space, detail::guarded("index creation", [&] {
                           return m_space->generateIndex(name);
                       }));
}

std::vector<IndexHandle> SpaceHandle::indices(std::size_t        count,
                                              std::string const &name) const
{
    if (name.empty())
        throw ValueError("indices: name must not be empty");
    auto const               raw = detail::guarded("index creation", [&] {
        return m_space->generateIndices(count, name);
    });
    std::vector<IndexHandle> result;
    result.reserve(raw.size());
    for (auto const &index : raw)
        result.emplace_back(m_space, index);
    return result;
}

IndexHandle SpaceHandle::index() const
{
    return IndexHandle(m_space, detail::guarded("index creation", [&] {
                           return m_space->generateIndex();
                       }));
}

std::vector<IndexHandle> SpaceHandle::indices(std::size_t count) const
{
    auto const raw = detail::guarded(
        "index creation", [&] { return m_space->generateIndices(count); });
    std::vector<IndexHandle> result;
    result.reserve(raw.size());
    for (auto const &index : raw)
        result.emplace_back(m_space, index);
    return result;
}

IndexHandle SpaceHandle::fixedIndex(int value) const
{
    if (value < 0 or value >= m_space->getDim())
        throw ValueError("fixed index: value must be in 0.."
                         + std::to_string(m_space->getDim() - 1));
    // CSL stores the value in a char: values above 127 would become negative
    // and reach a null space inside CSL.
    if (value > 127)
        throw ValueError("fixed index: value must be at most 127");
    return IndexHandle(m_space, detail::guarded("fixed index creation", [&] {
                           csl::Index index(static_cast<char>(value));
                           index.setSpace(m_space.get());
                           return index;
                       }));
}

csl::Space const &SpaceHandle::raw() const
{
    return *m_space;
}

std::shared_ptr<csl::Space const> const &SpaceHandle::owner() const
{
    return m_space;
}

// IndexHandle

IndexHandle::IndexHandle(std::shared_ptr<csl::Space const> owner,
                         csl::Index                        index)
    : m_owner(std::move(owner)), m_index(std::move(index))
{
}

bool IndexHandle::isFixed() const
{
    return m_index.getType() == cslIndex::Fixed;
}

bool IndexHandle::isDummy() const
{
    return m_index.getType() == cslIndex::Dummy;
}

IndexType IndexHandle::type() const
{
    requireSpace(m_index, "index type");
    if (isFixed())
        return IndexType::Fixed;
    if (isDummy())
        return IndexType::Dummy;
    return IndexType::Free;
}

unsigned int IndexHandle::id() const
{
    requireSpace(m_index, "index id");
    return m_index.getID();
}

std::string IndexHandle::name() const
{
    if (isFixed())
        throw TypeError("name: a fixed index has no name, use value()");
    return std::string(m_index.getName());
}

int IndexHandle::value() const
{
    if (not isFixed())
        throw TypeError("value: only a fixed index has a value");
    return static_cast<int>(m_index.getValue());
}

IndexHandle IndexHandle::flipped() const
{
    return IndexHandle(m_owner, m_index.getFlipped());
}

bool IndexHandle::equals(IndexHandle const &other) const
{
    return m_index == other.m_index;
}

IndexKey IndexHandle::hashKey() const
{
    IndexKey key;
    key.space     = reinterpret_cast<std::uintptr_t>(m_index.getSpace());
    key.type      = static_cast<int>(m_index.getType());
    key.magnitude = std::abs(static_cast<int>(m_index.getValue()));
    key.id = isFixed() ? 0U : static_cast<unsigned int>(m_index.getID());
    return key;
}

csl::Index const &IndexHandle::raw() const
{
    return m_index;
}

// TensorHandle

TensorHandle::TensorHandle(std::string const              &name,
                           std::vector<SpaceHandle> const &spaces,
                           SymmetrySpec const             &symmetric,
                           SymmetrySpec const             &antisymmetric,
                           csl::ComplexProperty            complexProperty)
    : m_spaces(spaces),
      m_tensor(makeTensor(
          name, m_spaces, symmetric, antisymmetric, complexProperty))
{
}

std::size_t TensorHandle::rank() const
{
    return m_spaces.size();
}

csl::Tensor const &TensorHandle::raw() const
{
    return m_tensor;
}

ExprHandle
TensorHandle::operator()(std::vector<IndexHandle> const &indices) const
{
    if (indices.size() != m_spaces.size())
        throw ValueError("tensor element: expected "
                         + std::to_string(m_spaces.size()) + " indices, got "
                         + std::to_string(indices.size()));
    std::vector<csl::Index> raw;
    raw.reserve(indices.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        if (indices[i].raw().getSpace() != &m_spaces[i].raw())
            throw ValueError("tensor element: index " + std::to_string(i)
                             + " belongs to another space");
        raw.push_back(indices[i].raw());
    }
    return detail::guarded("tensor element",
                           [&] { return ExprHandle((*m_tensor)(raw)); });
}

// TensorFieldHandle

TensorFieldHandle::TensorFieldHandle(std::string const &name,
                                     SpaceHandle const &pointSpace,
                                     std::vector<SpaceHandle> const &spaces)
    : m_pointSpace(pointSpace),
      m_spaces(spaces),
      m_field(makeField(name, m_pointSpace, m_spaces))
{
}

ExprHandle
TensorFieldHandle::operator()(std::vector<IndexHandle> const &indices,
                              TensorHandle const             &point) const
{
    if (indices.size() != m_spaces.size())
        throw ValueError("tensor field element: expected "
                         + std::to_string(m_spaces.size()) + " indices, got "
                         + std::to_string(indices.size()));
    std::vector<csl::Index> raw;
    raw.reserve(indices.size());
    for (std::size_t i = 0; i < indices.size(); ++i) {
        checkIndexSpace(
            indices[i], &m_spaces[i].raw(), "tensor field element");
        raw.push_back(indices[i].raw());
    }
    checkPoint(point, &m_pointSpace.raw(), "tensor field element");

    return detail::guarded("tensor field element", [&] {
        if (raw.empty())
            return ExprHandle((*m_field)(point.raw()));
        return ExprHandle((*m_field)(raw, point.raw()));
    });
}

// TensorDerivativeHandle

TensorDerivativeHandle::TensorDerivativeHandle(std::string const &name,
                                               SpaceHandle const &space)
    : m_space(space),
      m_derivative(detail::guarded("tensor derivative creation", [&] {
          return csl::TDerivative(name, &m_space.raw());
      }))
{
}

ExprHandle TensorDerivativeHandle::operator()(IndexHandle const  &index,
                                              TensorHandle const &point) const
{
    if (index.isFixed())
        throw ValueError("tensor derivative: index must be free or dummy");
    checkIndexSpace(index, &m_space.raw(), "tensor derivative");
    checkPoint(point, &m_space.raw(), "tensor derivative");

    return detail::guarded("tensor derivative element", [&] {
        return ExprHandle((*m_derivative)(index.raw(), point.raw()));
    });
}

ExprHandle applyTensorDerivative(ExprHandle const &derivative,
                                 ExprHandle const &operand)
{
    auto const *derivativeElement
        = dynamic_cast<csl::TDerivativeElement const *>(
            derivative.raw().get());
    if (derivativeElement == nullptr)
        throw TypeError("only a tensor derivative element can be called with "
                        "an operand");

    return detail::guarded("tensor derivative application", [&] {
        auto const *tensorElement
            = dynamic_cast<csl::TensorElement const *>(operand.raw().get());
        if (tensorElement != nullptr and tensorElement->getNIndices() == 1
            and derivativeElement->getPoint().get()
                    == tensorElement->getParent().get()) {
            auto const delta
                = csl::GetDelta(derivativeElement->getIndex(0).getSpace());
            return ExprHandle((*delta)(std::vector<csl::Index>{
                derivativeElement->getIndex(0), tensorElement->getIndex(0)}));
        }
        return ExprHandle(
            derivative.raw()->applyOperator(operand.raw(), false));
    });
}

// IndexRenamingHandle

void IndexRenamingHandle::add(IndexHandle const &original,
                              IndexHandle const &renamed)
{
    if (original.type() != IndexType::Free
        or renamed.type() != IndexType::Free) {
        throw ValueError("index renaming: only free indices can be renamed");
    }
    m_renaming.renamed[renamingKey(original.raw())] = renamed.raw();
}

std::vector<std::pair<IndexHandle, IndexHandle>>
IndexRenamingHandle::pairs() const
{
    std::vector<std::pair<IndexHandle, IndexHandle>> result;
    result.reserve(m_pairs.size());
    for (auto const &[old, fresh] : m_pairs)
        result.emplace_back(IndexHandle(nullptr, old),
                            IndexHandle(nullptr, fresh));
    return result;
}

// Free functions

ExprHandle metric(IndexHandle const &first, IndexHandle const &second)
{
    csl::Space const *space = commonSpace({first, second}, "metric");
    return detail::guarded("metric", [&] {
        auto tensor = csl::GetMetric(space);
        return ExprHandle(
            (*tensor)(std::vector<csl::Index>{first.raw(), second.raw()}));
    });
}

ExprHandle delta(IndexHandle const &first, IndexHandle const &second)
{
    csl::Space const *space = commonSpace({first, second}, "delta");
    return detail::guarded("delta", [&] {
        auto tensor = csl::GetDelta(space);
        return ExprHandle(
            (*tensor)(std::vector<csl::Index>{first.raw(), second.raw()}));
    });
}

ExprHandle epsilon(std::vector<IndexHandle> const &indices)
{
    if (indices.empty())
        throw ValueError("epsilon: at least one index is required");
    csl::Space const *space = commonSpace(indices, "epsilon");
    int const         dim   = space->getDim();
    if (dim < 2 or dim > 4)
        throw ValueError("epsilon: defined for spaces of dimension 2 to 4 "
                         "only");
    if (static_cast<int>(indices.size()) != dim)
        throw ValueError("epsilon: expected one index per dimension ("
                         + std::to_string(dim) + "), got "
                         + std::to_string(indices.size()));
    std::vector<csl::Index> raw;
    raw.reserve(indices.size());
    for (auto const &index : indices)
        raw.push_back(index.raw());
    return detail::guarded("epsilon", [&] {
        auto tensor = csl::GetEpsilon(space);
        return ExprHandle((*tensor)(raw));
    });
}

ExprHandle relabelIndices(ExprHandle const &expr, bool relabelFree)
{
    IndexRenamingHandle renaming;
    return relabelIndices(expr, relabelFree, renaming);
}

ExprHandle relabelIndices(ExprHandle const    &expr,
                          bool                 relabelFree,
                          IndexRenamingHandle &renaming)
{
    std::vector<csl::Index> originals;
    for (auto const &index : freeIndices(expr))
        originals.push_back(index.raw());

    ExprHandle relabelled = detail::guarded("index relabelling", [&] {
        return ExprHandle(
            csl::RelabelIndices(expr.raw(), relabelFree, renaming.m_renaming));
    });

    renaming.m_pairs.clear();
    std::set<csl::IndexRenaming::Key> seen;
    for (auto const &original : originals) {
        auto const key   = renamingKey(original);
        auto const found = renaming.m_renaming.renamed.find(key);
        if (found != renaming.m_renaming.renamed.end()
            and seen.insert(key).second)
            renaming.m_pairs.emplace_back(original, found->second);
    }
    return relabelled;
}

std::vector<IndexHandle> freeIndices(ExprHandle const &expr)
{
    return detail::guarded("free indices", [&] {
        auto const structure = expr.raw()->getFreeIndexStructure();
        return distinctIndices(structure.getIndexView(), cslIndex::Free);
    });
}

std::vector<IndexHandle> dummyIndices(ExprHandle const &expr)
{
    return detail::guarded("dummy indices", [&] {
        auto const structure = expr.raw()->getIndexStructure();
        return distinctIndices(structure.getIndexView(), cslIndex::Dummy);
    });
}

} // namespace csl::api
