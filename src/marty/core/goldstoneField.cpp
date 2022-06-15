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

#include "goldstoneField.h"
#include "vectorField.h"

namespace mty {

GoldstoneBoson::GoldstoneBoson(
    std::shared_ptr<VectorBoson> const &t_vectorBoson)
    : GoldstoneBoson("G_" + std::string(t_vectorBoson->getName()),
                     t_vectorBoson)
{
}

GoldstoneBoson::GoldstoneBoson(
    std::string const &                 t_name,
    std::shared_ptr<VectorBoson> const &t_vectorBoson)
    : ScalarBoson(t_name,
                  t_vectorBoson->getGaugeIrrep(),
                  t_vectorBoson->isSelfConjugate()),
      vectorBoson(t_vectorBoson)
{
    setLatexName("G_" + t_vectorBoson->getLatexName());
}

void GoldstoneBoson::printDefinition(std::ostream &out,
                                     int           indentSize,
                                     bool          header) const
{
    std::string indent(indentSize, ' ');
    std::string regName = csl::Abstract::regularName(name);
    out << indent;
    if (header)
        out << "inline ";
    out << "mty::Particle " << regName << " = ";
    out << "mty::goldstoneboson_s(\"" << csl::Abstract::regularLiteral(name)
        << " ; " << csl::Abstract::regularLiteral(latexName) << "\", ";
    out << "std::dynamic_pointer_cast<mty::VectorBoson>("
        << csl::Abstract::regularName(vectorBoson.lock()->getName())
        << "));\n";
    out << indent << "model->addParticle(" << regName << ");\n";
}

Particle GoldstoneBoson::getVectorBoson() const
{
    return vectorBoson.lock();
}

Particle GoldstoneBoson::generateSimilar(std::string const &t_name) const
{
    return csl::make_shared<GoldstoneBoson>(t_name, vectorBoson.lock());
}

int GoldstoneBoson::getNDegreesOfFreedom() const
{
    return vectorBoson.lock()->getNDegreesOfFreedom();
}

Particle goldstoneboson_s(std::string const &                 name,
                          std::shared_ptr<VectorBoson> const &t_vectorBoson)
{
    std::shared_ptr<GoldstoneBoson> goldstone
        = csl::make_shared<GoldstoneBoson>(name, t_vectorBoson);
    t_vectorBoson->setGoldstoneBoson(goldstone);
    return goldstone;
}

Particle goldstoneboson_s(std::shared_ptr<VectorBoson> const &t_vectorBoson)
{
    std::shared_ptr<GoldstoneBoson> goldstone
        = csl::make_shared<GoldstoneBoson>(t_vectorBoson);
    t_vectorBoson->setGoldstoneBoson(goldstone);
    return goldstone;
}

Particle goldstoneboson_s(std::string const &name,
                          Particle const &   t_vectorBoson)
{
    HEPAssert(IsOfType<VectorBoson>(t_vectorBoson)
                  or IsOfType<GaugeBoson>(t_vectorBoson),
              mty::error::TypeError,
              "Goldstone boson must be created from a vector boson, "
                  + toString(t_vectorBoson->getName()) + " given instead.");
    return goldstoneboson_s(name, ConvertToShared<VectorBoson>(t_vectorBoson));
}

Particle goldstoneboson_s(Particle const &t_vectorBoson)
{
    HEPAssert(IsOfType<VectorBoson>(t_vectorBoson)
                  or IsOfType<GaugeBoson>(t_vectorBoson),
              mty::error::TypeError,
              "Goldstone boson must be created from a vector boson, "
                  + toString(t_vectorBoson->getName()) + " given instead.");
    return goldstoneboson_s(ConvertToShared<VectorBoson>(t_vectorBoson));
}

} // namespace mty
