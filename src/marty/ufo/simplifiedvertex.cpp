#include "simplifiedvertex.h"
#include "electriccharge.h"
#include "parsefactors.h"
#include "vertexinfo.h"

namespace mty::experimental::ufo
{

    /////////////////////////////////////////////////////
    // Helper functions for the main convertion function
    // See the definitions at the bottom of the file
    /////////////////////////////////////////////////////

    std::vector<Vertex> convertFeynmanRule(
        csl::Expr expr,
        VertexMessenger &messenger);

    std::vector<Vertex> expandAndConvertFeynmanRule(
        csl::Expr expr,
        VertexMessenger &messenger);

    mty::experimental::ufo::Field convertField(
        mty::QuantumField const &field,
        VertexMessenger const &messenger);

    void extractFields(
        VertexMessenger &messenger);

    void extractColorSpaces(
        VertexMessenger &messenger);

    bool requiresExpansion(
        csl::Expr const &term,
        VertexMessenger const &messenger);

    void decomposeContribution(
        csl::Expr term,
        VertexMessenger &messenger);

    std::vector<DiracMatrix> decomposeDiracCoupling(
        csl::Expr const &diracCoupling);

    DiracMatrix::Type getGammaMatrixType(
        sgl::GExpr gammaMatrix);

    /////////////////////////////////////////////////////
    // Main convertion functions
    /////////////////////////////////////////////////////

    std::vector<Vertex> convertFeynmanRule(
        mty::FeynmanRule const &rule,
        mty::Model &model,
        SMData const &smData)
    {
        Vertex vertex;
        VertexMessenger messenger{vertex, rule, model, smData};
        csl::Expr expr = messenger.rule.getExpr();
        csl::Evaluate(expr, csl::eval::abbreviation);
        csl::DeepHardFactor(expr);
        return convertFeynmanRule(expr, messenger);
    }

    std::vector<Vertex> convertFeynmanRule(
        csl::Expr expr,
        VertexMessenger &messenger)
    {
        extractFields(messenger);
        extractColorSpaces(messenger);
        if (requiresExpansion(expr, messenger))
        {
            return expandAndConvertFeynmanRule(expr, messenger);
        }
        else
        {
            decomposeContribution(expr, messenger);
            return {messenger.vertex};
        }
    }

    std::vector<Vertex> convertFeynmanRules(
        std::vector<FeynmanRule> const &rules,
        mty::Model &model,
        SMData const &smData)
    {
        std::vector<Vertex> convertedVertices;
        convertedVertices.reserve(rules.size());
        for (const auto &rule : rules)
        {
            std::vector<Vertex> vertices = convertFeynmanRule(rule, model, smData);
            convertedVertices.insert(
                convertedVertices.end(),
                std::make_move_iterator(vertices.begin()),
                std::make_move_iterator(vertices.end()));
        }
        return convertedVertices;
    }

    std::vector<Vertex> obtainSimplifiedVertices(
        mty::Model &model,
        SMData const &smData)
    {
        auto const &feynmanRules = model.getFeynmanRules();
        return convertFeynmanRules(
            feynmanRules,
            model,
            smData);
    }

    /////////////////////////////////////////////////////
    // Helper functions for the main convertion function
    // See the declarations at the top of the file
    /////////////////////////////////////////////////////

    Field convertField(
        mty::QuantumField const &field,
        VertexMessenger const &messenger)
    {
        std::string const &colorName = messenger.smData.colorName;
        auto const *colorGroup = colorName.empty() ? nullptr : messenger.model.getGroup(colorName);
        std::string const &photonName = messenger.smData.photonName;
        Field ufoField;
        ufoField.name = field.getName();
        ufoField.momentum = field.getPoint()->getName();
        ufoField.spinDimension = field.getSpinDimension();
        if (field.isBosonic() && field.isAntiCommuting())
        {                                // ghost
            ufoField.spinDimension = -1; // Specific UFO conventio for ghosts
        }
        if (!photonName.empty())
        {
            ufoField.electricCharge = getElectricCharge(
                ElectricChargeMessenger{
                    field.getQuantumParent(),
                    messenger.model.getParticle(photonName).get(),
                    messenger.model,
                    messenger.smData});
        }
        else
        {
            ufoField.electricCharge = Charge{0, 1};
        }
        ufoField.colorDimension = colorGroup ? field.getGroupIrrep(colorGroup).getDim() : 1;
        ufoField.antiParticle = field.isComplexConjugate();
        ufoField.indices = field.getIndexStructure();

        return ufoField;
    }

    void extractFields(
        VertexMessenger &messenger)
    {
        std::vector<mty::QuantumField> const &fields = messenger.rule.getFieldProduct();
        messenger.vertex.fields.resize(fields.size());
        for (size_t i = 0; i != fields.size(); ++i)
        {
            messenger.vertex.fields[i] = convertField(fields[i], messenger);
        }
    }

    void extractColorSpaces(
        VertexMessenger &messenger)
    {
        auto colorName = messenger.smData.colorName;
        if (colorName.empty())
            return;
        auto colorGroup = messenger.model.getGaugedGroup(colorName);
        auto group = colorGroup->getGroup();
        for (const auto &field : messenger.vertex.fields)
        {
            mty::Irrep colorRep = messenger.model.getGroupIrrep(
                field.name, colorName);
            if (colorRep.getDim() > 1)
            {
                messenger.colorSpaces.push_back(
                    group->getVectorSpace(colorRep));
            }
        }
    }

    bool requiresExpansion(
        csl::Expr const &expr,
        VertexMessenger const &messenger)
    {
        auto sumMustBeExpanded = [&](csl::Expr const &sum)
        {
            bool lorentz = false;
            bool color = false;
            for (const auto &arg : sum)
            {
                if (containsDiracFactor(arg))
                {
                    return true; // Always expand gamma matrix
                }
                if (containsColorFactor(arg, messenger.colorSpaces))
                {
                    color = true;
                }
                if (containsLorentzFactor(arg))
                {
                    lorentz = true;
                }
            }
            return lorentz && color;
        };

        return csl::AnyOfNodes(expr, [&](csl::Expr const &term)
                               { return csl::IsSum(term) && sumMustBeExpanded(term); });
    }

    std::vector<Vertex> expandAndConvertFeynmanRule(
        csl::Expr expr,
        VertexMessenger &messenger)
    {
        csl::DeepExpand(expr);
        if (!csl::IsSum(expr))
        {
            CallHEPError(mty::error::RuntimeError,
                         "Expanded a term that must not be: " + toString(expr))
        }
        std::vector<Vertex> result;
        result.reserve(expr->size());
        for (const auto &arg : expr)
        {
            Vertex newVertex = messenger.vertex;
            VertexMessenger newMessenger = messenger;
            newMessenger.vertex = newVertex;
            std::vector<Vertex> interm = convertFeynmanRule(arg, newMessenger);
            if (interm.size() != 1)
            {
                CallHEPError(mty::error::RuntimeError,
                             "Could not properly parse " + toString(arg))
            }
            result.push_back(interm[0]);
        }
        return result;
    }

    void decomposeContribution(
        csl::Expr expr,
        VertexMessenger &messenger)
    {
        FactorDecomposition decompo = decomposeExpression(
            expr, messenger.colorSpaces);
        messenger.vertex.color.generators = decompo.color;
        messenger.vertex.coupling = decompo.coupling;
        messenger.vertex.lorentz.lorentzFactor = decompo.lorentz;
        messenger.vertex.lorentz.gammaMatrices = decomposeDiracCoupling(decompo.dirac);
    }

    std::vector<DiracMatrix> decomposeDiracCoupling(
        csl::Expr const &diracCoupling)
    {
        sgl::TensorSet tset{
            dirac4.gamma_chir,
            dirac4.C_matrix,
            dirac4.P_L,
            dirac4.P_R,
            {}};
        tset.gamma[0] = dirac4.getDelta();
        tset.gamma[1] = dirac4.gamma;
        tset.gamma[2] = dirac4.sigma;
        sgl::GExpr gammaExpr = sgl::csl_to_sgl(diracCoupling, tset);
        auto chain = std::dynamic_pointer_cast<sgl::IndexChain>(
            gammaExpr);
        if (!chain)
        {
            HEPAssert(diracCoupling == CSL_1,
                      mty::error::RuntimeError,
                      "Unrecognized gamma-matrix chain " + toString(diracCoupling)) return {};
        }
        auto [a, b] = chain->getBorderIndices();
        std::vector<csl::Index> indices = dirac4.generateIndices(1 + chain->size());
        indices[0] = a;
        indices.back() = b;
        std::vector<DiracMatrix> finalChain(chain->size());
        for (size_t i = 0; i != chain->size(); ++i)
        {
            finalChain[i] = DiracMatrix{
                getGammaMatrixType(chain->argument(i)),
                csl::IndexStructure(chain->argument(i)->indices()),
                indices[i],
                indices[i + 1]};
        }
        return finalChain;
    }

    DiracMatrix::Type getGammaMatrixType(
        sgl::GExpr gammaMatrix)
    {
        auto gamma = std::dynamic_pointer_cast<sgl::GammaIndex>(gammaMatrix);
        if (gamma->isDelta())
            return DiracMatrix::Identity;
        if (gamma->isGammaMu())
            return DiracMatrix::Gamma;
        if (gamma->isGamma5())
            return DiracMatrix::Gamma5;
        if (gamma->isSigma())
            return DiracMatrix::Sigma;
        if (gamma->isC())
            return DiracMatrix::C;
        if (gamma->isP_L())
            return DiracMatrix::P_L;
        if (gamma->isP_R())
            return DiracMatrix::P_R;
        CallHEPError(
            mty::error::RuntimeError,
            "Gamma matrix " + toString(gammaMatrix) + " not recognized") return DiracMatrix::Identity;
    }

} // namespace mty::experimental::ufo
