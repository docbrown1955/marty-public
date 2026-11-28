#include <gtest/gtest.h>
#include <marty.h>

using namespace csl;
using namespace mty;

namespace {

QuantumField quantumFieldOf(Expr const &expr)
{
    QuantumField field;
    Expr         copy = expr;
    FirstOfLeaf(copy, [&](Expr const &sub) {
        if (IsOfType<QuantumField>(sub)) {
            field = ConvertTo<QuantumField>(sub);
            return true;
        }
        return false;
    });
    return field;
}

std::vector<QuantumFieldParent const *>
fermionParents(std::vector<QuantumField> const &fields)
{
    std::vector<QuantumFieldParent const *> res;
    for (auto const &field : fields)
        if (field.isFermionic())
            res.push_back(field.getQuantumParent());
    return res;
}

} // namespace

TEST(marty_fermion_order, user_order_keeps_fermion_insertions_in_place)
{
    Model    model(std::vector<GaugeGroupSpec>{});
    Particle psi = diracfermion_s("psi", model);
    Particle lam = diracfermion_s("lam", model);
    Particle phi = scalarboson_s("phi", model);
    model.addParticles({psi, lam, phi});

    Index                     a = DiracIndex();
    std::vector<QuantumField> fields{
        quantumFieldOf(psi(a)), quantumFieldOf(phi()), quantumFieldOf(lam(a))};
    std::vector<Insertion> insertions{
        Insertion(psi), Insertion(phi), Insertion(lam)};
    std::vector<QuantumFieldParent const *> expected{psi.get(), lam.get()};

    FeynOptions options;
    options.keepFermionInsertionOrder = true;
    options.setFermionOrder({1, 0});
    Kinematics kinematics(insertions);
    kinematics.setExternalSpinTensors(
        std::vector<Expr>(insertions.size(), CSL_UNDEF));
    AmplitudeInitializer::orderInsertions(fields, kinematics, options);

    EXPECT_EQ(fermionParents(fields), expected);
    EXPECT_EQ(options.getFermionOrder(), (std::vector<int>{1, 0}));
}
