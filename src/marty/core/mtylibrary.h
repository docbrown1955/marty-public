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
 * @version 1.3

 * \brief
 */
#pragma once

#include "../../csl/csl.h"
#include "model.h"

namespace mty {

class Library : public csl::LibraryGenerator {
  public:
    template <class... Args>
    Library(Args &&...args)
        : csl::LibraryGenerator(std::forward<Args>(args)...)
    {
        addInclude("clooptools.h");
        addInclude("marty/core/looptools_init.h");
#if !(defined __APPLE__ || defined __MACH__)
        // If we are not on MacOS, add -lgfortran
        addLibrary("-lgfortran");
        setGccCompiler();
#else
        setClangCompiler();
#endif
        if (csl::LibraryGenerator::isQuadruplePrecision()) {
            addLibrary("-looptools-quad");
            addInclude("marty/core/looptools_quad_extension.h");
            addLibrary("-lquadmath");
        }
        else {
            addLibrary("-looptools");
        }
    }

    void applyDiagonalizationData(Model &model)
    {
        model.applyDiagonalizationData(*this);
    }

    void generateSpectrum(Model &model)
    {
        model.applyDiagonalizationData(*this);
        for (const auto &m : model.getAbbreviatedMassExpressions()) {
            if (csl::Abbrev::find_opt(m)) {
                addMassExpression(m->getName());
                addFunction(m->getName(), m, "G");
            }
        }
        for (const auto &particle : model.getParticles()) {
            csl::Expr mass  = particle->getMass();
            csl::Expr width = particle->getWidth();
            if (csl::IsLiteral(mass)) {
                addDefaultParameter(mass->getName(),
                                    mass->getComplexProperty()
                                        == csl::ComplexProperty::Complex);
            }
            if (csl::IsLiteral(width)) {
                addDefaultParameter(width->getName(),
                                    width->getComplexProperty()
                                        == csl::ComplexProperty::Complex);
            }
        }
    }

    void importLHAModule(std::string const &t_pathToMarty)
    {
        lhaEnabled  = true;
        pathToMarty = t_pathToMarty;
    }

    void addFunction(std::string const &name,
                     csl::Expr          expr,
                     std::string const &groupName = "G")
    {
        csl::ScopedProperty p(&csl::option::checkCommutations, false);
        auto &f = csl::LibraryGenerator::addFunction(name, expr, groupName);
        f.addInitInstruction("clearcache();");
    }

    void print()
    {
        if (lhaEnabled)
            doImportLHAModule();
        csl::LibraryGenerator::print();
    }

    void build(unsigned int nJobs = 1)
    {
        if (lhaEnabled)
            doImportLHAModule();
        csl::LibraryGenerator::build(nJobs);
    }

  private:
    void doImportLHAModule()
    {
        csl::LibraryGenerator::setupDirectory();
        const auto           martySrc  = pathToMarty + "/src";
        const auto           martyInc  = pathToMarty + "/include";
        const auto           targetSrc = path + "/src";
        const auto           targetInc = path + "/include";
        [[maybe_unused]] int sysres
            = system(("mkdir -p " + martySrc + " " + martyInc).c_str());
        sysres = system(("cp " + martySrc + "/lha.cpp " + targetSrc).c_str());
        sysres
            = system(("cp " + martySrc + "/lhaData.cpp " + targetSrc).c_str());
        sysres = system(("cp " + martyInc + "/lha.h " + targetInc).c_str());
        sysres
            = system(("cp " + martyInc + "/lhaData.h " + targetInc).c_str());
        sysres
            = system(("cp " + martyInc + "/lhaBlocks.h " + targetInc).c_str());
        sysres = system(("cp " + pathToMarty + "/../csl/include/std_vector*.h "
                         + targetInc)
                            .c_str());
    }

    bool        lhaEnabled{false};
    std::string pathToMarty;
};
} // namespace mty
