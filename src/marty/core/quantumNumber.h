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

/*! \file
 * \author Gregoire Uhlrich
 * \version 1.3
 * \brief Contains the QuantumNumber class.
 */
#ifndef QUANTUM_NUMBER_H_INCLUDED
#define QUANTUM_NUMBER_H_INCLUDED

#include <map>
#include <string>
#include <vector>

namespace mty {

class QuantumField;

/*!
 * \brief This class represents quantum numbers of a theory, like for example
 * baryon number B, lepton number L etc.
 * \details When doing a calculation, you may give a list of independant
 * quantum numbers (like B and L in the SM) to be conserved. This will not
 * change the result but will allow MARTY to discard early diagrams that are
 * zero. You must not give non-independant quantum numbers like F (fermion
 * number) and B. If so, MARTY will discard diagrams that are actually possible
 * because some particles (quarks in this example) may contribute in both
 * numbers.
 */
class QuantumNumber {

  public:
    /*!
     * \brief Integer type for QuantumNumber id.
     */
    using ID = unsigned short;
    /*!
     * \brief Integer type for quantum number value.
     */
    using Value = int;

  private:
    /*!
     * \brief Current id of QuantumNumber.
     * \details This is used to define the id of new quantum numbers. Each
     * time, this variable is incremented.
     */
    static inline ID current_id = 0;

    /*!
     * \brief Tells if the quantum number is conserved.
     * \details True by default. If false, the quantum number has no impact on
     * any calculation.
     */
    bool conserved;

    /*!
     * \brief ID of the quantum number.
     */
    ID id;

  private:
    /*!
     * \brief Returns the map between ids and names for quantumNumbers.
     * \details This function allocates statically a map between IDs and names,
     * and returns a reference to it.
     * \return The mapping between ids and names for quantum numbers.
     */
    static inline std::map<ID, std::string> &name()
    {
        static std::map<ID, std::string> m;
        return m;
    }

  public:
    /*!
     * \brief Constructor with 2 parameters.
     * \details Initliazes the name, and possible the conserved property of the
     * quantum number.
     *
     * \param t_name      Name.
     * \param t_conserved Boolean that tells if the quantum number is conserved
     * (default = true).
     */
    explicit QuantumNumber(std::string const &t_name, bool t_conserved = true);

    /*!
     * \brief Defaulted copy constructor.
     */
    QuantumNumber(QuantumNumber const &) = default;
    /*!
     * \brief Defaulted copy assignement operator.
     */
    QuantumNumber &operator=(QuantumNumber const &) = default;

    /*!
     * \brief Destructor.
     */
    ~QuantumNumber();

    /*!
     * \brief Returns the conservation property of the quantum number.
     * \return \b True  if the quantum number is conserved.
     * \return \b False else.
     */
    bool isConserved() const;

    /*!
     * \brief Returns the name of the quantum number.
     * \details The name is not stored directly in the quantum number. Instead,
     * the id is the key to get it. This function then determines the name
     * corresponding to the quantum number's id, and returns it.
     * \return The name of the quantum number.
     */
    std::string getName() const;

    /*!
     * \brief Sets the conserved property of the quantum number.
     * \param t_conserved Boolean. If true, the quantum number is conserved and
     * may be used to simplify calculations.
     */
    void setConservedProperty(bool t_conserved);

    /*!
     * \brief Sets the name of the quantum number.
     * \param t_name New name of the quantum number.
     */
    void setName(std::string const &t_name);

    /*!
     * \brief Tells if one of the fields has a non trivial value for the
     * quantum number. \param fields List of fields. \return \b True  if one of
     * the fields has a non trivial value for the quantum number. \return \b
     * False else.
     */
    bool appearsIn(std::vector<mty::QuantumField> const &fields) const;

    /*!
     * \brief Calculates the value of the total quantum numbers carried by a
     * list of fields.
     * \details For example, qqq has baryonic number B = 1, leptonic number
     * L = 0, fermionic number F = 3.
     * \param fields List of fields
     * \return The sum of the quantum values corresponding to the fields.
     */
    int
    computeQuantumNumber(std::vector<mty::QuantumField> const &field) const;

    /*!
     * \brief Returns the id of the quantum number.
     * \return The quantum number ID
     */
    inline ID getID() const
    {
        return id;
    }

    /*!
     * \brief Compares the ids of two quantum numbers.
     * \return \b True  if the two quantum numbers have the same id (same
     * quantum number then)
     * \return \b False else
     */
    inline bool operator==(QuantumNumber const &other) const
    {
        return id == other.id;
    }

    /*!
     * \brief Compares the ids of two quantum numbers.
     * \return \b False if the two quantum numbers have the same id (same
     * quantum number then)
     * \return \b True  else
     */
    inline bool operator!=(QuantumNumber const &other) const
    {
        return id != other.id;
    }
};

} // namespace mty

#endif
