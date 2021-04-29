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

/*!
 * @file 
 * @author Grégoire Uhlrich
 * @version 1.0
 
 * \brief
 */
#ifndef MTY_GENERATOR_H_INCLUDED
#define MTY_GENERATOR_H_INCLUDED

#include <csl.h>
#include "representation.h"

namespace mty {

class GaugedGroup;

class GeneratorParent :public csl::TensorParent {

public:

    template<class ...Args>
    GeneratorParent(
            mty::GaugedGroup *t_group,
            mty::Irrep const &t_irrep,
            Args            &&...args)
        :csl::TensorParent(std::forward<Args>(args)...),
        group(t_group),
        irrep(t_irrep)
    {}

    void printDefinition(
            std::ostream &out        = std::cout,
            int           indentSize = 4,
            bool          header = false
            ) const override;

    csl::Expr operator()(csl::Index index) override;

    csl::Expr operator()(std::vector<csl::Index> indices) override;

    csl::Expr operator()(const std::vector<int>& indices) override;

    std::string getCodeName() const;

private:

    mty::GaugedGroup *group;
    mty::Irrep        irrep;
};


class GeneratorElement: public csl::TensorElement {

public:

    template<class ...Args>
    GeneratorElement(Args &&...args)
        :csl::TensorElement(std::forward<Args>(args)...)
    {}

    csl::unique_Expr copy_unique() const override;

    csl::Expr refresh() const override;

    void printCode(
            int           mode = 0,
            std::ostream &out  = std::cout
            ) const override;
};

template<class ...Args>
std::shared_ptr<GeneratorParent> generator_s(
        Args &&...args
        )
{
    return csl::make_shared<GeneratorParent>(
            std::forward<Args>(args)...
            );
}

template<class ...Args>
csl::Expr generatorelement_s(
        Args &&...args
        )
{
    return csl::make_shared<GeneratorElement>(
            std::forward<Args>(args)...)
        ->getCanonicalPermutation();
}

class Generator: public std::shared_ptr<GeneratorParent> {

public:

    INHERIT_SHARED_PTR_CONSTRUCTOR(Generator, GeneratorParent)

    /*! \brief Constructor of a 1-indexed parent.
     * \param t_group Group of the generator.
     * \param t_irrep Irreducible representation of the generator.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the Index lives.
     */
    Generator(
            mty::GaugedGroup *t_group,
            mty::Irrep const &t_irrep,
            const std::string& t_name,
            const csl::Space* t_space
            )
        :Generator(generator_s(t_group, t_irrep, t_name, t_space))
    {}

    /*! \brief Constructor of a n-indexed parent.
     * \param t_group Group of the generator.
     * \param t_irrep Irreducible representation of the generator.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     */
    Generator(
            mty::GaugedGroup *t_group,
            mty::Irrep const &t_irrep,
            const std::string& t_name,
            const std::vector<const csl::Space*>& t_space
            )
        :Generator(generator_s(t_group, t_irrep, t_name, t_space))
    {}

    /*! \brief Constructor of a n-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_group Group of the generator.
     * \param t_irrep Irreducible representation of the generator.
     * \param t_name Name of the parent.
     * \param t_space List of the n spaces in which each index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    Generator(
            mty::GaugedGroup *t_group,
            mty::Irrep const &t_irrep,
            const std::string& t_name,
            const std::vector<const csl::Space*>& t_space,
            const csl::Expr& t_tensor
            )
        :Generator(generator_s(t_group, t_irrep, t_name, t_space, t_tensor))
    {}

    /*! \brief Constructor of a 1-indexed parent, with explicit tensor 
     * \b tensor.
     * \param t_group Group of the generator.
     * \param t_irrep Irreducible representation of the generator.
     * \param t_name Name of the parent.
     * \param t_space Vector space in which the index lives.
     * \param t_tensor Explicit tensor. Must match exactly the shape given by
     * the IndexStructure.
     */
    Generator(
            mty::GaugedGroup *t_group,
            mty::Irrep const &t_irrep,
            const std::string& t_name,
            const csl::Space* t_space,
            const csl::Expr& t_tensor
            )
        :Generator(generator_s(t_group, t_irrep, t_name, t_space, t_tensor))
    {}


    template<class ...Args>
        inline
        csl::Expr operator()(Args&& ...args)
        {
            return (**this)(std::forward<Args>(args)...);
        }

    template<class ...Args>
        inline
        csl::Expr operator()(const std::vector<int>& indices, Args&& ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }

    template<class ...Args>
        inline
        csl::Expr operator()(const std::vector<csl::Index>& indices, Args&& ...args)
        {
            return (**this)(indices, std::forward<Args>(args)...);
        }
};

DEFINE_SHARED_PTR_OPERATOR(Generator)

} // End of namespace mty

#endif
