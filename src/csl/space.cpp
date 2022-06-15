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

#include "space.h"
#include "counter.h"
#include "error.h"
#include <limits>

using namespace std;

namespace csl {

bool Space::applyMetric = true;

Space::Space(const std::string &             t_name,
             int                             t_dim,
             std::vector<std::string> const &defaultNames)
    : name(t_name),
      dim(t_dim),
      signedIndex(false),
      defaultIndexNames(defaultNames),
      delta(csl::make_shared<DeltaParent>(this)),
      metric(delta),
      inverseMetric(delta)
{
    CSL_ASSERT_SPEC(not defaultIndexNames.empty(),
                    CSLError::ValueError,
                    "You must provide at least one default index name for the "
                    "space "
                        + name + ".");

    defaultName = defaultIndexNames.begin();
    buildEpsilon();
}

// const Space with metric
Space::Space(const std::string &             t_name,
             int                             t_dim,
             const std::string &             name_metric,
             const Expr &                    t_metric,
             std::vector<std::string> const &defaultNames)
    : name(t_name),
      dim(t_dim),
      signedIndex(true),
      defaultIndexNames(defaultNames),
      delta(csl::make_shared<DeltaParent>(this)),
      metric((t_metric == CSL_UNDEF)
                 ? csl::make_shared<MetricParent>(this, name_metric)
                 : csl::make_shared<MetricParent>(this, t_metric, name_metric))
{
    CSL_ASSERT_SPEC(not defaultIndexNames.empty(),
                    CSLError::ValueError,
                    "You must provide at least one default index name for the "
                    "space "
                        + name + ".");

    defaultName = defaultIndexNames.begin();
    if (t_metric == CSL_UNDEF)
        inverseMetric = csl::make_shared<MetricParent>(this, name_metric);
    else
        inverseMetric = csl::make_shared<MetricParent>(
            this, t_metric->inverseMatrix(), name_metric);
    buildEpsilon();
}

Space::~Space()
{
}

void Space::printCode(std::ostream &out, int indentSize) const
{
    std::string indent(indentSize, ' ');
    out << indent << csl::Abstract::regularName(name);
}

void Space::printDefinition(std::ostream &out,
                            int           indentSize,
                            bool          header) const
{
    std::string indent(indentSize, ' ');
    auto        keyword = (header) ? "inline " : "static ";
    out << indent << keyword << "std::unique_ptr<csl::Space> "
        << csl::Abstract::regularName(name)
        << "_uptr = std::make_unique<csl::Space>(\n";
    out << indent << "    "
        << "\"" << csl::Abstract::regularLiteral(name) << "\",\n";
    out << indent << "    " << dim << ",\n";
    if (signedIndex) {
        out << indent << "    "
            << "\"" << csl::Abstract::regularLiteral(metric->getName())
            << "\",\n";
        out << indent << "    ";
        metric->getTensor()->printCode(1, out);
        out << ",\n";
    }
    out << indent << "    "
        << "std::vector<std::string>({";
    for (const auto &n : defaultIndexNames)
        out << "\"" << csl::Abstract::regularLiteral(n) << "\", ";

    out << "})\n";
    out << indent << "    "
        << ");\n";
    keyword = (header) ? "inline " : "";
    out << indent << keyword << "csl::Space *"
        << csl::Abstract::regularName(name) << " = "
        << csl::Abstract::regularName(name) << "_uptr.get();\n";
}

void Space::buildEpsilon()
{
    epsilon  = csl::make_shared<EpsilonParent>(this);
    Expr det = metric->getTensor()->determinant();
    if (dim < 5 and dim >= 2) {
        vector<Index> indices(dim);
        for (auto &i : indices)
            i = generateIndex();
        vector<Index> indices2(dim);
        for (auto &i : indices2)
            i = generateIndex();
        if (signedIndex)
            indices2[0] = indices[0].getFlipped();
        else
            indices2[0] = indices[0];

        vector<size_t>   perm = csl::range(dim - 1);
        csl::vector_expr res;
        res.reserve(factorial(dim - 1));
        int    sign  = 1;
        size_t count = 1;
        do {
            Expr   term = CSL_1;
            size_t i    = 0;
            for (size_t p : perm) {
                term = term * metric({indices[1 + i++], indices2[1 + p]});
            }
            res.push_back(sign * term);
            if (++count % 2 == 0)
                sign = -sign;
        } while (std::next_permutation(perm.begin(), perm.end()));

        epsilon->addSelfContraction(
            epsilon(indices), epsilon(indices2), Expanded(det * sum_s(res)));
    }
}

std::string Space::getName() const
{
    return name;
}

bool Space::hasSpecialTraceProperty(const csl::vector_expr &) const
{
    callError(cslError::AbstractFuncCalled,
              "Space::hasSpecialTraceProperty(const csl::vector_expr& "
              "tensors) const");
    return false;
}

csl::vector_expr Space::getSignature() const
{
    Expr             metricTensor = metric->getTensor();
    csl::vector_expr res(metricTensor->size());
    for (int i = 0; i != static_cast<int>(res.size()); ++i)
        res[i] = metricTensor->getArgument({i, i});

    return res;
}

Tensor Space::getMetric() const
{
    return metric;
}

Tensor Space::getInverseMetric() const
{
    return inverseMetric;
}

Tensor Space::getDelta() const
{
    return delta;
}

Tensor Space::getEpsilon() const
{
    return epsilon;
}

Expr Space::applyMetricOnTensor(Expr const &tensor,
                                size_t      axis,
                                bool        covariant) const
{
    Expr appliedTensor
        = (covariant) ? metric->getTensor() : inverseMetric->getTensor();
    // A_i1 j ... in = g_j i2 * A_i1 i2 ... in \forall j

    Expr              newTensor = highdtensor_s(tensor->getShape());
    std::vector<int>  pos(tensor->getDim());
    std::list<size_t> max;
    std::vector<int>  shape = tensor->getShape();
    for (int i : shape)
        max.push_back(static_cast<size_t>(i));
    counter<size_t> count(max);
    count.restart();

    do {
        count.getCount(pos.begin());
        Expr arg = tensor->getArgument(pos);
        if (arg != CSL_0) {
            int j = pos[axis];
            for (int i = 0; i < dim; ++i) {
                auto factor = appliedTensor->getArgument({j, i});
                if (factor != CSL_0) {
                    pos[axis] = i;
                    Expr el   = newTensor->getArgument(pos);
                    el        = sum_s(el, factor * arg);
                    newTensor->setArgument(el, pos);
                }
            }
        }
    } while (++count);

    return newTensor;
}

Expr Space::generateVector(const std::string &t_name) const
{
    return generateTensor(getProperVectorName(t_name), {this});
}

string Space::generateVectorName(const string &t_name) const
{
    return getProperVectorName(t_name);
}

void Space::addVectorNames(const string &name) const
{
    addVectorNames(vector<string>(1, name));
}

void Space::addIndexNames(const string &name) const
{
    addIndexNames(vector<string>(1, name));
}

void Space::addVectorNames(const std::vector<std::string> &names) const
{
    for (const auto &name : names) {
        if (findFirstInPair(availableVectors, name) != availableVectors.end())
            continue;
        availableVectors.push_back(make_pair(name, 0));
    }
}

void Space::addIndexNames(const std::vector<std::string> &names) const
{
    for (const auto &name : names) {
        if (findFirstInPair(availableIndices, name) != availableIndices.end())
            continue;
        availableIndices.push_back(make_pair(name, 0));
    }
}

string Space::getProperVectorName(const string &initialName) const
{
    addVectorNames(initialName);
    auto iter   = findFirstInPair(availableVectors, initialName);
    int  number = (*iter).second;
    if (number != 0) {
        ostringstream sout;
        sout << initialName << "_" << number;
        ++((*iter).second);
        return sout.str();
    }
    ++((*iter).second);
    return initialName;
}

Expr Space::calculateTrace(csl::vector_expr) const
{
    callError(cslError::AbstractFuncCalled,
              "Space::calculateTrace(const csl::vector_expr& tensors) const");
    return CSL_UNDEF;
}

void fillEpsilonTensor(Expr &tensor, int dim)
{
    vector<int> indices(dim);
    for (int i = 0; i != dim; ++i)
        indices[i] = i;

    size_t parity = 0;
    Expr   value  = CSL_1;
    do {
        if (++parity % 2 == 0)
            value = -value;
        tensor->setArgument(value, indices);
    } while (std::next_permutation(indices.begin(), indices.end()));
}

} // End of namespace csl
