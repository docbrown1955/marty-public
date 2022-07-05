#include "electriccharge.h"
#include "parsefactors.h"

namespace mty::experimental::ufo
{

    std::optional<mty::FeynmanRule const *> electricChargeVertex(
        ElectricChargeMessenger const &messenger)
    {
        for (const auto &rule : messenger.model.getFeynmanRules())
        {
            std::vector<mty::QuantumField> const &content = rule.getFieldProduct();
            if (content.size() != 3)
                continue;
            bool matter = false;
            bool antiMatter = false;
            bool boson = false;
            for (const auto &field : content)
            {
                auto const *parent = field.getQuantumParent();
                if (parent == messenger.bosonField)
                {
                    boson = true;
                }
                else if (parent == messenger.matterField)
                {
                    if (field.isComplexConjugate())
                        antiMatter = true;
                    else
                        matter = true;
                }
            }
            if (matter && antiMatter && boson)
                return &rule;
        }
        return std::nullopt;
    }

    ufo::Charge getElectricCharge(
        ElectricChargeMessenger const &messenger)
    {
        if (dynamic_cast<WeylFermion const *>(messenger.matterField))
        {
            auto newMessenger = messenger;
            newMessenger.matterField = messenger.matterField->getDiracParent().get();
            return getElectricCharge(newMessenger);
        }
        else if (dynamic_cast<FieldStrength const *>(messenger.matterField))
        {
            auto newMessenger = messenger;
            newMessenger.matterField = messenger.matterField->getVectorBoson().get();
            return getElectricCharge(newMessenger);
        }
        std::optional<mty::FeynmanRule const *> optChargeVertex = electricChargeVertex(messenger);
        if (!optChargeVertex)
            return {0, 1};
        mty::FeynmanRule const &chargeVertex = *(optChargeVertex.value());
        switch (messenger.matterField->getSpinDimension())
        {
        case 1:
            return getScalarElectricCharge(chargeVertex, messenger);
        case 2:
            return getFermionElectricCharge(chargeVertex, messenger);
        case 3:
            return getVectorElectricCharge(chargeVertex, messenger);
        default:
            raiseElectricChargeError(
                {CSL_UNDEF,
                 "Error: Field " + messenger.matterField->getName() + " of spin " + std::to_string(messenger.matterField->getSpinDimension()) + " is not recognized for electric charge caculation."});
        }
        return ufo::Charge{0, 1};
    }

    ufo::Charge getScalarElectricCharge(
        mty::FeynmanRule const &vertexRule,
        ElectricChargeMessenger const &messenger)
    {
        // Parses terms of the type
        // A(mu) * d_mu(phi) * phi^*
        // or the hermitic conjugate. Depending on the conjugation of the
        csl::Expr term = getTerm(vertexRule, messenger);
        csl::Expr sign = CSL_UNDEF;
        for (const auto &arg : term)
        {
            if (csl::IsTensorialDerivative(arg))
            {
                csl::Expr field = arg[0];
                if (field->isComplexConjugate())
                    sign = CSL_M_1;
                else
                    sign = CSL_1;
            }
        }
        if (sign == CSL_UNDEF)
        {
            raiseElectricChargeError({term,
                                      "Ill-defined term, should have tensor derivative."});
        }
        csl::Expr charge = sign * getConstantFactor(term) / CSL_I;
        return exprToCharge(charge);
    }

    ufo::Charge getFermionElectricCharge(
        mty::FeynmanRule const &vertexRule,
        ElectricChargeMessenger const &messenger)
    {
        // Parses terms of the type
        // A(mu) * psi^* gamma(mu) psi
        csl::Expr term = getTerm(vertexRule, messenger);
        csl::Expr charge = getConstantFactor(term);
        if (!csl::IsNumerical(charge))
        {
            raiseElectricChargeError(
                {term,
                 "The charge should be numerical, found " + toString(charge)});
        }
        return exprToCharge(charge);
    }

    csl::Expr getVectorElectricCharge(
        csl::Expr const &term,
        ElectricChargeMessenger const &messenger)
    {
        // Parses terms of the type
        // A(mu) * B(nu) * F_B(mu, nu)^*
        // A is the boson field, B is the matter field (field strength F_B)
        // A possible sign comes from index ordering in F_B
        if (!csl::IsProd(term))
        {
            raiseElectricChargeError({term, "Expected a product."});
        }
        csl::Index matterIndex;
        csl::Index bosonIndex;
        auto bosonParent = messenger.bosonField;
        auto matterParent = messenger.matterField->getFieldStrength().get();
        for (const auto &arg : term)
        {
            if (csl::IsIndicialTensor(arg))
            {
                auto parent = arg->getParent().get();
                if (parent == matterParent)
                {
                    matterIndex = arg->getIndexStructureView().back();
                }
                else if (parent == bosonParent)
                {
                    bosonIndex = arg->getIndexStructureView().back();
                }
            }
        }
        if (matterIndex == csl::Index() || bosonIndex == csl::Index())
        {
            raiseElectricChargeError({term,
                                      "Ill-defined contraction indices for vector em charge vertex."});
        }
        csl::Expr sign = (matterIndex == bosonIndex) ? CSL_M_1 : CSL_1;
        csl::Expr charge = sign * getConstantFactor(term) / CSL_I;
        return charge;
    }

    ufo::Charge getVectorElectricCharge(
        mty::FeynmanRule const &vertexRule,
        ElectricChargeMessenger const &messenger)
    {
        csl::Expr term = csl::Expanded(getTerm(vertexRule, messenger));
        if (csl::IsSum(term))
        {
            std::vector<csl::Expr> chargeElements(term->size());
            for (size_t i = 0; i != chargeElements.size(); ++i)
            {
                chargeElements[i] = getVectorElectricCharge(term[i], messenger);
            }
            return exprToCharge(csl::sum_s(chargeElements));
        }
        else
        {
            return exprToCharge(getVectorElectricCharge(term, messenger));
        }
    }

    csl::Expr getTerm(
        mty::FeynmanRule const &vertex,
        ElectricChargeMessenger const &messenger)
    {
        csl::Expr term = csl::DeepCopy(vertex.getInteractionTerm()->getTerm());
        if (!csl::IsProd(term))
        {
            raiseElectricChargeError({term, "The term should be a product."});
        }
        for (auto &arg : term)
        {
            if (arg->getName() == messenger.smData.emCouplingName)
            {
                arg = CSL_1;
                return csl::Refreshed(term);
            }
        }
        raiseElectricChargeError({term,
                                  "Expected electromagnetic coupling \"" + messenger.smData.emCouplingName + "\"."});
        return CSL_0;
    }

    ufo::Charge exprToCharge(csl::Expr charge)
    {
        csl::DeepRefresh(charge);
        if (csl::IsInteger(charge))
        {
            int value = std::round(charge->evaluateScalar());
            return ufo::Charge{value, 1};
        }
        else if (csl::IsIntFraction(charge))
        {
            return ufo::Charge{
                static_cast<int>(charge->getNum()),
                static_cast<int>(charge->getDenom())};
        }
        raiseElectricChargeError(
            {charge,
             "The final charge should be a rational number."});
        return ufo::Charge{0, 1};
    }

    void raiseElectricChargeError(
        ElectricChargeError const &error)
    {
        if (error.term != CSL_UNDEF)
        {
            std::cerr << "Error: Electric charge for term\n";
            std::cerr << error.term << std::endl;
            std::cerr << "is not recognized." << std::endl;
        }
        CallHEPError(mty::error::RuntimeError, error.message)
    }

} // namespace mty::experimental::ufo
