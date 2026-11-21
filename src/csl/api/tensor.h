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
 * @file tensor.h
 * @brief Python neutral handles on CSL spaces, indices and tensors.
 *
 * Handles keep their CSL space alive through a `std::shared_ptr`. An
 * `ExprHandle` holds only a CSL expression, which refers to spaces by raw
 * pointer: it does not keep them alive.
 */

#ifndef CSL_API_TENSOR_H_INCLUDED
#define CSL_API_TENSOR_H_INCLUDED

#include "../enum.h"        // csl::ComplexProperty
#include "../index.h"       // csl::Index
#include "../indicial.h"    // csl::Tensor
#include "../interface.h"   // csl::IndexRenaming
#include "../space.h"       // csl::Space
#include "../tensorField.h" // csl::TensorField, csl::TDerivative
#include "expr.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace csl::api {

class IndexHandle;

/*! \brief Identity of an index, used for hashing.
 *
 * Two indices that compare equal have equal keys. The key ignores the sign
 * of free and dummy indices, as `csl::Index::operator==` does.
 */
struct IndexKey {
    std::uintptr_t space     = 0;
    int            type      = 0;
    int            magnitude = 0;
    unsigned int   id        = 0;

    bool operator==(IndexKey const &other) const = default;
};

/*! \brief Index space. Owns its CSL space through a shared pointer. */
class SpaceHandle {
  public:
    /*! \brief Build a space of dimension \p dimension with the given index
     * names. */
    SpaceHandle(std::string const              &name,
                int                             dimension,
                std::vector<std::string> const &indexNames);

    /*! \brief Minkowski space (signed, dimension 4). */
    [[nodiscard]] static SpaceHandle minkowski();
    /*! \brief Euclidean space of dimension 2. */
    [[nodiscard]] static SpaceHandle euclid2();
    /*! \brief Euclidean space of dimension 3. */
    [[nodiscard]] static SpaceHandle euclid3();
    /*! \brief Euclidean space of dimension 4. */
    [[nodiscard]] static SpaceHandle euclid4();

    [[nodiscard]] int dimension() const;
    /*! \brief Name of the space. */
    [[nodiscard]] std::string name() const;

    /*! \brief Free index with name \p name. */
    [[nodiscard]] IndexHandle index(std::string const &name) const;

    /*! \brief Free index with the next default name of the space. */
    [[nodiscard]] IndexHandle index() const;

    /*! \brief \p count distinct free indices with name \p name. */
    [[nodiscard]] std::vector<IndexHandle>
    indices(std::size_t count, std::string const &name) const;

    /*! \brief \p count distinct free indices with default names. */
    [[nodiscard]] std::vector<IndexHandle> indices(std::size_t count) const;

    /*! \brief Fixed index with value \p value, in 0..dimension-1 and at
     * most 127. */
    [[nodiscard]] IndexHandle fixedIndex(int value) const;

    [[nodiscard]] csl::Space const                        &raw() const;
    [[nodiscard]] std::shared_ptr<csl::Space const> const &owner() const;

    /*! \brief Handle on an existing space; \p space keeps the space alive
     * (an aliasing pointer can share the lifetime of its owner). */
    explicit SpaceHandle(std::shared_ptr<csl::Space const> space);

  private:
    std::shared_ptr<csl::Space const> m_space;
};

/*! \brief Type of an index. */
enum class IndexType { Free, Dummy, Fixed };

/*! \brief Index of a space.
 *
 * The owner keeps the space alive. Indices returned by freeIndices() and
 * dummyIndices() have no owner: their space is only kept alive by the
 * caller.
 */
class IndexHandle {
  public:
    IndexHandle(std::shared_ptr<csl::Space const> owner, csl::Index index);

    [[nodiscard]] bool isFixed() const;
    [[nodiscard]] bool isDummy() const;

    /*! \brief Type of the index. ValueError if the index has no space. */
    [[nodiscard]] IndexType type() const;

    /*! \brief CSL identifier of the index. ValueError if the index has no
     * space. */
    [[nodiscard]] unsigned int id() const;

    /*! \brief Name of a free or dummy index. TypeError for fixed indices. */
    [[nodiscard]] std::string name() const;

    /*! \brief Value of a fixed index. TypeError for free or dummy indices. */
    [[nodiscard]] int value() const;

    [[nodiscard]] IndexHandle flipped() const;
    [[nodiscard]] bool        equals(IndexHandle const &other) const;
    [[nodiscard]] IndexKey    hashKey() const;

    [[nodiscard]] csl::Index const &raw() const;

  private:
    std::shared_ptr<csl::Space const> m_owner;
    csl::Index                        m_index;
};

/*! \brief Symmetry declaration: full symmetry, or a list of position pairs.
 */
struct SymmetrySpec {
    bool                                             fully = false;
    std::vector<std::pair<std::size_t, std::size_t>> pairs;
};

/*! \brief Indexed tensor with one space per index position. */
class TensorHandle {
  public:
    /*! \brief Build a tensor. Conflicting symmetries raise ValueError before
     * the CSL tensor is created. The complex property is Real by default. */
    TensorHandle(std::string const              &name,
                 std::vector<SpaceHandle> const &spaces,
                 SymmetrySpec const             &symmetric     = {},
                 SymmetrySpec const             &antisymmetric = {},
                 csl::ComplexProperty            complexProperty
                 = csl::ComplexProperty::Real);

    /*! \brief Wraps an existing CSL tensor whose index positions have the
     * spaces \p spaces (for example a tensor returned by the model). */
    TensorHandle(csl::Tensor tensor, std::vector<SpaceHandle> spaces);

    [[nodiscard]] std::size_t rank() const;

    /*! \brief Name of the tensor. */
    [[nodiscard]] std::string name() const;
    /*! \brief Index spaces, one per index position. */
    [[nodiscard]] std::vector<SpaceHandle> const &spaces() const;
    /*! \brief True if all index positions are symmetric. */
    [[nodiscard]] bool fullySymmetric() const;
    /*! \brief True if all index positions are antisymmetric. */
    [[nodiscard]] bool fullyAntiSymmetric() const;

    /*! \brief Element of the tensor. The indices must match the spaces of the
     * tensor, position by position. */
    [[nodiscard]] ExprHandle
    operator()(std::vector<IndexHandle> const &indices) const;

    [[nodiscard]] csl::Tensor const &raw() const;

  private:
    std::vector<SpaceHandle> m_spaces;
    csl::Tensor              m_tensor;
};

/*! \brief Square tensor on the space \p space, with the given diagonal and
 * zero off-diagonal entries (as a numerical matrix, so that the core can see
 * it is diagonal). */
TensorHandle diagonalTensor(std::string const             &name,
                            SpaceHandle const             &space,
                            std::vector<ExprHandle> const &diagonal);

/*! \brief Unitary square tensor on the space \p space, whose numerical value
 * is \p rows (one vector per row). ValueError if \p rows is not square of the
 * dimension of \p space. */
TensorHandle unitaryTensor(std::string const                          &name,
                           SpaceHandle const                          &space,
                           std::vector<std::vector<ExprHandle>> const &rows);

/*! \brief Tensor field: a tensor that depends on a point of a space.
 *
 * The field has one index space per index position, given by \p spaces (none
 * for a scalar field), and a point space. Calling the field with the indices
 * and a rank-1 point tensor of the point space gives a field element.
 */
class TensorFieldHandle {
  public:
    TensorFieldHandle(std::string const              &name,
                      SpaceHandle const              &pointSpace,
                      std::vector<SpaceHandle> const &spaces);

    /*! \brief Name of the field. */
    [[nodiscard]] std::string name() const;
    /*! \brief Space of the point the field depends on. */
    [[nodiscard]] SpaceHandle const &pointSpace() const;
    /*! \brief Index spaces, one per index position (empty for a scalar). */
    [[nodiscard]] std::vector<SpaceHandle> const &indexSpaces() const;
    /*! \brief Primary type of the field (csl::PrimaryType::Field). */
    [[nodiscard]] csl::PrimaryType primaryType() const;

    /*! \brief Element of the field at \p point. ValueError if the indices do
     * not match the index spaces, or if the point is not a rank-1 tensor of
     * the point space. */
    [[nodiscard]] ExprHandle
    operator()(std::vector<IndexHandle> const &indices,
               TensorHandle const             &point) const;

  private:
    SpaceHandle              m_pointSpace;
    std::vector<SpaceHandle> m_spaces;
    csl::TensorField         m_field;
};

/*! \brief Tensorial derivative with respect to a space.
 *
 * Calling it with an index and a rank-1 point tensor of the space gives a
 * derivative element, which is then applied to an operand with
 * applyTensorDerivative().
 */
class TensorDerivativeHandle {
  public:
    TensorDerivativeHandle(std::string const &name, SpaceHandle const &space);

    /*! \brief Derivative element d(index, point). ValueError if the index is
     * fixed or does not belong to the space, or if the point is not a rank-1
     * tensor of the space. */
    [[nodiscard]] ExprHandle operator()(IndexHandle const  &index,
                                        TensorHandle const &point) const;

  private:
    SpaceHandle      m_space;
    csl::TDerivative m_derivative;
};

/*! \brief Applies a derivative element to \p operand, as CSL applyOperator()
 * does. TypeError unless \p derivative is a tensorial derivative element. */
[[nodiscard]] ExprHandle applyTensorDerivative(ExprHandle const &derivative,
                                               ExprHandle const &operand);

/*! \brief Renaming of the free indices of an expression, read and filled by
 * relabelIndices().
 *
 * Each pair holds an index of the input expression (old) and its new index.
 * The indices of pairs() have no owner: their space is only kept alive by the
 * caller.
 */
class IndexRenamingHandle {
  public:
    /*! \brief Sets the new index of the free index \p original. Used as input
     * by relabelIndices(): a free index of the expression keeps this new
     * index. ValueError if either index is not free. */
    void add(IndexHandle const &original, IndexHandle const &renamed);

    /*! \brief (old, new) pairs of the free indices, in order of first
     * occurrence in the input. Dummy indices are not included. */
    [[nodiscard]] std::vector<std::pair<IndexHandle, IndexHandle>>
    pairs() const;

  private:
    friend ExprHandle relabelIndices(ExprHandle const    &expr,
                                     bool                 relabelFree,
                                     IndexRenamingHandle &renaming);

    csl::IndexRenaming                             m_renaming;
    std::vector<std::pair<csl::Index, csl::Index>> m_pairs;
};

/*! \brief Metric tensor element, same space for both indices. */
[[nodiscard]] ExprHandle metric(IndexHandle const &first,
                                IndexHandle const &second);

/*! \brief Kronecker delta element, same space for both indices. */
[[nodiscard]] ExprHandle delta(IndexHandle const &first,
                               IndexHandle const &second);

/*! \brief Levi-Civita symbol, for spaces of dimension 2 to 4. The number of
 * indices must equal the dimension of the space. */
[[nodiscard]] ExprHandle epsilon(std::vector<IndexHandle> const &indices);

/*! \brief Rename indices of an expression to canonical names. */
[[nodiscard]] ExprHandle relabelIndices(ExprHandle const &expr,
                                        bool              relabelFree = true);

/*! \brief Same as relabelIndices(expr, relabelFree), with renaming as input
 * and output. Indices of \p expr already in \p renaming keep their new index,
 * the others get canonical names. Both are added to \p renaming. Entries for
 * other indices are kept. */
[[nodiscard]] ExprHandle relabelIndices(ExprHandle const    &expr,
                                        bool                 relabelFree,
                                        IndexRenamingHandle &renaming);

/*! \brief Free indices of an expression, without duplicates. */
[[nodiscard]] std::vector<IndexHandle> freeIndices(ExprHandle const &expr);

/*! \brief Dummy indices of an expression, without duplicates. */
[[nodiscard]] std::vector<IndexHandle> dummyIndices(ExprHandle const &expr);

/*! \brief Number of indices of an indicial expression, 0 for any other
 * expression. */
[[nodiscard]] std::size_t indexCount(ExprHandle const &expr);

/*! \brief Index at \p index of an indicial expression. LookupError when out
 * of range, including for expressions that are not indicial. */
[[nodiscard]] IndexHandle indexAt(ExprHandle const &expr, std::size_t index);

/*! \brief Raw index structure of an expression, in CSL order, with
 * repetitions. Empty for an expression without indices. */
[[nodiscard]] std::vector<IndexHandle> indexStructure(ExprHandle const &expr);

} // namespace csl::api

#endif
