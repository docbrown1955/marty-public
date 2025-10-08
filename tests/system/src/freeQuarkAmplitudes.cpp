/*
 * This program calculates the gg -> ttbar squared amplitude at the tree-level
 * in the Standard Model. Ghost calculations are performed to remove
 * un-physical polarizations from the results.
 */
#include "comparedata.h"
#include "marty/models/sm.h"
#include "testutility.h"
#include <marty.h>

using namespace std;
using namespace csl;
using namespace mty;

// Find implementation below main()
mty::WilsonSet split_quark_contributions(mty::Model const     &model,
                                         mty::WilsonSet const &wilsons,
                                         std::string const &color_group_name,
                                         std::string const &quark_name);

int main()
{
    mty::sm_input::redefineNumericalValues(); // for compatibility
    mty::sm_input::g_s->setValue(CSL_UNDEF);
    mty::sm_input::m_t->setValue(CSL_UNDEF);
    SM_Model sm;
    Display(sm);

    auto rules = sm.getFeynmanRules();
    Display(rules);

    mty::Amplitude ampl = sm.computeAmplitude(Order::TreeLevel,
                                              {Incoming("G"),
                                               Incoming("G"),
                                               Outgoing("t"),
                                               Outgoing(AntiPart("t"))});
    Display(ampl);

    WilsonSet wilsons = sm.getWilsonCoefficients(ampl);
    Display(wilsons);
    // Operators are of the form
    // G^A_mu G^B_nu p^i_rho (tbar gamma^mu gamma^nu gamma^rho T^A T^B t)
    // with
    //   - p^i_rho being one of the 3 independent momenta (p1, p2, p3)
    //   - p_i being in contracted to one of three gammas
    //   - T^A, T^B the SU(3) generators in some order (or equivalently the
    //   gluon Minkowski contraction order)
    // Total: 3*3*2 = 18 operators
    if (wilsons.size() != 18) {
        throw std::runtime_error("Error: expected 18 operators, found "
                                 + to_string(wilsons.size()));
    }

    WilsonSet split_wilsons = split_quark_contributions(sm, wilsons, "C", "t");

    Display(split_wilsons);
    // After the split we should have 18*3*3 = 162 operators (3*3 from color
    // indices of the top quark)
    if (split_wilsons.size() != 162) {
        throw std::runtime_error("Error: expected 162 operators after color "
                                 "split, found "
                                 + to_string(split_wilsons.size()));
    }

    DisplayAbbreviations();

    return 0;
}

mty::WilsonSet split_quark_contributions(mty::Model const     &model,
                                         mty::WilsonSet const &wilsons,
                                         std::string const &color_group_name,
                                         std::string const &quark_name)
{
    csl::Space const *color_space
        = model.getVectorSpace(color_group_name, quark_name);
    mty::WilsonSet split_wilsons;
    for (const mty::Wilson &wilson : wilsons) {
        const mty::WilsonOperator op   = wilson.op;
        const Expr                expr = op.getExpression();
        std::vector<csl::Index>   quark_color_indices;
        csl::VisitEachLeaf(expr, [&](const Expr &leaf) {
            if (leaf->isIndexed()) {
                // It is a top quark expression (external leg)
                csl::IndexStructure const &indices
                    = leaf->getIndexStructureView();
                for (const csl::Index &ind : indices.getIndex()) {
                    if (ind.getSpace() == color_space
                        and std::find(quark_color_indices.begin(),
                                      quark_color_indices.end(),
                                      ind)
                                == quark_color_indices.end()) {
                        quark_color_indices.push_back(ind);
                    }
                }
            }
        });
        if (quark_color_indices.size() != 2) {
            Display(expr);
            std::cout << "Found " << quark_color_indices.size()
                      << " color indices for the top quark." << std::endl;
            for (const csl::Index &ind : quark_color_indices) {
                std::cout << "  - " << ind << " in space "
                          << ind.getSpace()->getName() << std::endl;
            }
            throw std::runtime_error("Error: could not find 2 different color "
                                     "indices for the top "
                                     "quark in diagram.");
        }
        csl::Expr split_expr = CSL_0;
        for (int i = 0; i != 3; ++i) {
            for (int j = 0; j != 3; ++j) {
                csl::Expr  contrib = csl::DeepCopy(expr);
                csl::Index ind_i   = quark_color_indices[0];
                ind_i.setValue(i);
                csl::Index ind_j = quark_color_indices[1];
                ind_j.setValue(j);
                csl::Replace(contrib, quark_color_indices[0], ind_i);
                csl::Replace(contrib, quark_color_indices[1], ind_j);
                split_wilsons.push_back(
                    mty::Wilson{wilson.coef, {contrib, op.getFactor()}});
            }
        }
    }
    return split_wilsons;
}
