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

/*! \file jsonToPhysics.h
 * \author Gregoire Uhlrich
 * \version 1.0
 * \brief File that contains the JSONToHEP class, allowing to load a HEP model
 * from a .json file.
 */
#ifndef JSONTOPHYSICS_H_INCLUDED
#define JSONTOPHYSICS_H_INCLUDED

#include <optional>
#include <memory>
#include "jsonLoader.h"

namespace csl {
    class Expr;
}

namespace mty {

class QuantumNumber;
class QuantumFieldParent;
class Flavor;
class FlavorIrrep;
class Gauge;
class GaugeIrrep;
class Model;

/*!
 * 
 * \brief Reads a file in .json format containing data of a model and returns
 * the corresponding Model object via its only static public function readModel.
 * \param nameFile The name of the .json file.
 * \return The Model filled with the data.
 */
class JSONToHEP {

    public:

    JSONToHEP() = delete;

    public:

    /*!
     * \brief Reads a file in .json format containing data of a model and
     * returns the corresponding Model object.
     * \param nameFile The name of the .json file.
     * \return The Model filled with the data.
     */
    static std::unique_ptr<Model> readModel(std::string const& nameFile);

    private:

    /*!
     * \brief Reads the gauge in the root node of the JSON tree and returns it.
     * \param tree Pointer to to root Node.
     * \return A raw pointer to a newly allocated Gauge.
     */
    static std::unique_ptr<Gauge> readGauge(JSON::Node* tree);

    /*!
     * \brief Reads the flavor in the root node of the JSON tree and returns it.
     * \param tree Pointer to to root Node.
     * \return A raw pointer to a newly allocated Flavor.
     */
    static std::unique_ptr<Flavor> readFlavor(JSON::Node* tree);

    static std::vector<QuantumNumber> readQuantumNumbers(JSON::Node* tree);

    /*!
     * \brief Reads the gauge coupling in a group Node and returns the 
     * corresponding csl csl::Expr.
     * \param couplingNode Pointer to the coupling Node.
     * \return The expression (aka csl::Expr) corresponding to the coupling.
     */
    static csl::Expr readGaugeCoupling(JSON::Node* couplingNode);

    /*!
     * \brief Reads the fields of the model in the root Node of the JSON tree
     * and returns it in the form of a std::vector of QuantumFieldParent*. The
     * parents are newly allocated and only raw pointers are returned.
     * \param tree Root Node of the file.
     * \param gauge Pointer the the model's gauge to initialize the fields.
     * \return a std::vector of QuantumFieldParent* containing the particles.
     */
    static std::vector<std::shared_ptr<QuantumFieldParent>>
        readField(JSON::Node* tree,
                  Gauge*      gauge,
                  Flavor*     flavor,
                  Model*      model,
                  std::vector<QuantumNumber> const& qNumbers);

    static std::vector<std::pair<std::string, JSON::Node*>>
        readYukawaNodes(JSON::Node* fields);

    static csl::Expr interpretYukawaCoupling(Model*             model,
                                        std::string const& nameScalar,
                                        JSON::Node*        node);

    /*!
     * \brief Little helper function that converts a string to a vector of int,
     * corresponding to the dinkin labels of a given representation.
     * \param str String to convert.
     * \return a std::vector of int containing the labels.
     */
    static std::vector<int> stringToDinkinLabels(std::string const& str);

    /*!
     * \brief Reads the irreducible representation of a particle in its Node.
     * \details For each group, the set of dinkin labels (or numerator and 
     * denominator for U(1)) must be given. If not, the trivial representation
     * is supposed for this group. The representation is then created at the 
     * gauge level.
     * \param node Node of the representation.
     * \param gauge Pointer to the gauge to initialize properly the
     * representations.
     * \return A GaugeIrrep read in the file.
     */
    static GaugeIrrep readIrrep(JSON::Node* node,
                                Gauge*      gauge);

    /*!
     * \brief Reads the irreducible representation of a particle in its Node.
     * \details For each group, the set of dinkin labels (or numerator and 
     * denominator for U(1)) must be given. If not, the trivial representation
     * is supposed for this group. The representation is then created at the 
     * gauge level.
     * \param node Node of the representation.
     * \param gauge Pointer to the gauge to initialize properly the
     * representations.
     * \return A GaugeIrrep read in the file.
     */
    static FlavorIrrep readFlavorIrrep(JSON::Node* node,
                                       Flavor*     flavor);

    /*!
     * \brief Reads the mass in a field Node and returns the 
     * corresponding csl csl::Expr.
     * \param massNode Pointer to the mass Node.
     * \return The expression (aka csl::Expr) corresponding to the mass.
     */
    static void readMass(JSON::Node*         massNode,
                         QuantumFieldParent* field);
};


} // End of namespace mty

#endif
