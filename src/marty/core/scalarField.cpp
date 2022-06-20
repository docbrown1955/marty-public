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

#include "scalarField.h"
#include "model.h"
#include "quantumFieldTheory.h"
namespace mty {

ScalarBoson::ScalarBoson(const std::string &t_name, Gauge *t_gauge)
    : QuantumFieldParent(t_name, 0, t_gauge)
{
    initPropagator();
}

ScalarBoson::ScalarBoson(const std::string &t_name, Model const &t_model)
    : ScalarBoson(t_name, t_model.getGauge())
{
    if (t_model.getFlavor()) {
        flavor    = t_model.getFlavor();
        flavorRep = flavor->getTrivialRep();
    }
}

ScalarBoson::ScalarBoson(const std::string &t_name, const GaugeIrrep &irrep)
    : QuantumFieldParent(t_name, 0, irrep)
{
    initPropagator();
}

ScalarBoson::ScalarBoson(const std::string &t_name,
                         Gauge *            t_gauge,
                         bool               t_isSelfConjugate)
    : QuantumFieldParent(t_name, 0, t_gauge, t_isSelfConjugate)
{
    initPropagator();
}

ScalarBoson::ScalarBoson(const std::string &t_name,
                         const GaugeIrrep & irrep,
                         bool               t_isSelfConjugate)
    : QuantumFieldParent(t_name, 0, irrep, t_isSelfConjugate)
{
    initPropagator();
}

ScalarBoson::ScalarBoson(const std::string &t_name,
                         const GaugeIrrep & irrep,
                         const FlavorIrrep &flavorRep)
    : QuantumFieldParent(t_name, 0, irrep, flavorRep)
{
    initPropagator();
}

ScalarBoson::ScalarBoson(const std::string &t_name, const ScalarBoson *other)
    : ScalarBoson(*other)
{
    setName(t_name);
    initPropagator();
}

void ScalarBoson::printDefinition(std::ostream &out,
                                  int           indentSize,
                                  bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName;
    out << " = mty::scalarboson_s(\"" << csl::Abstract::regularLiteral(name)
        << " ; " << csl::Abstract::regularLiteral(latexName)
        << "\", _model_);\n";
    printQuantumDefinition(out, indentSize);
}

Particle ScalarBoson::generateSimilar(std::string const &t_name) const
{
    return csl::make_shared<ScalarBoson>(t_name, this);
}

csl::Expr ScalarBoson::getEnergyDimension() const
{
    return CSL_1;
}

int ScalarBoson::getNDegreesOfFreedom() const
{
    return getNGaugeDegreesOfFreedom();
}

csl::Expr ScalarBoson::getKineticTerm(csl::Tensor &X)
{
    return ScalarKineticTerm(*this, X);
}

void ScalarBoson::initPropagator()
{
    propagator.clear();
    if (not integratedOut)
        propagator[this] = &ScalarPropagator;
    else
        propagator[this] = &IntegratedScalarPropagator;
}

} // namespace mty
