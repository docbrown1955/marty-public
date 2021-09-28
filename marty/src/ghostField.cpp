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

#include "ghostField.h"
#include "vectorField.h"
#include "quantumFieldTheory.h"

namespace mty {

GhostBoson::GhostBoson(
        std::shared_ptr<VectorBoson> const& t_vectorBoson)
    :GhostBoson("c_" + std::string(t_vectorBoson->getName()),
                  t_vectorBoson)
{
}

GhostBoson::GhostBoson(
        std::string const& t_name,
        std::shared_ptr<VectorBoson> const& t_vectorBoson)
    :ScalarBoson(t_name, t_vectorBoson->getGaugeIrrep()),
    vectorBoson(t_vectorBoson)
{
    setMass(t_vectorBoson->getMass() 
          * csl::sqrt_s(t_vectorBoson->getXiGauge()));
    setDrawType(drawer::ParticleType::Ghost);
}

GhostBoson::GhostBoson(
        std::string const &t_name,
        GaugeIrrep  const &rep
        )
    :ScalarBoson(t_name, rep)
{
    setDrawType(drawer::ParticleType::Ghost);
}

void GhostBoson::printDefinition(
        std::ostream &out,
        int           indentSize,
        bool          header
        ) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName << " = ";
    auto boson = vectorBoson.lock();
    out << "mty::ghostboson_s(\"" << csl::Abstract::regularLiteral(name) 
        << " ; " << csl::Abstract::regularLiteral(latexName)
        << "\", std::dynamic_pointer_cast<mty::VectorBoson>(" 
        << csl::Abstract::regularName(boson->getName())
        << "));\n";
    out << indent << "model->addParticle(" << regName << ", false);\n";
}

Particle GhostBoson::getVectorBoson() const
{
    return vectorBoson.lock();
}

Particle GhostBoson::generateSimilar(std::string const& t_name) const
{
    return csl::make_shared<GhostBoson>(t_name, getGaugeIrrep());
}

csl::Expr GhostBoson::getKineticTerm(csl::Tensor & X)
{
    return GhostKineticTerm(*this, X);
}

bool GhostBoson::isAntiCommuting() const
{
    return true;
}

int GhostBoson::getNDegreesOfFreedom() const
{
    return vectorBoson.lock()->getNDegreesOfFreedom();
}

Particle ghostboson_s(
        std::string                 const& name,
        std::shared_ptr<VectorBoson> const& t_vectorBoson,
        bool                                conjugated
        )
{
    std::shared_ptr<GhostBoson> goldstone = 
        csl::make_shared<GhostBoson>(name, t_vectorBoson);
    if (conjugated)
        t_vectorBoson->setConjugatedGhostBoson(goldstone);
    else
        t_vectorBoson->setGhostBoson(goldstone);
    return goldstone;
}

Particle ghostboson_s(
        std::shared_ptr<VectorBoson> const& t_vectorBoson,
        bool                                conjugated
        )
{
    std::shared_ptr<GhostBoson> goldstone = 
        csl::make_shared<GhostBoson>(t_vectorBoson);
    if (conjugated)
        t_vectorBoson->setConjugatedGhostBoson(goldstone);
    else
        t_vectorBoson->setGhostBoson(goldstone);
    return goldstone;
}

Particle ghostboson_s(
        std::string const& name,
        Particle    const& t_vectorBoson,
        bool               conjugated
        )
{
    HEPAssert(IsOfType<VectorBoson>(t_vectorBoson)
            or IsOfType<GaugeBoson>(t_vectorBoson),
            mty::error::TypeError,
            "Goldstone boson must be created from a vector boson, "
            + toString(t_vectorBoson->getName()) + " given instead.");
    return ghostboson_s(name,
                        ConvertToShared<VectorBoson>(t_vectorBoson),
                        conjugated);
}

Particle ghostboson_s(
        Particle const& t_vectorBoson,
        bool            conjugated
        )
{
    HEPAssert(IsOfType<VectorBoson>(t_vectorBoson)
            or IsOfType<GaugeBoson>(t_vectorBoson),
            mty::error::TypeError,
            "Goldstone boson must be created from a vector boson, "
            + toString(t_vectorBoson->getName()) + " given instead.");
    return ghostboson_s(
            ConvertToShared<VectorBoson>(t_vectorBoson),
            conjugated);
}


}
