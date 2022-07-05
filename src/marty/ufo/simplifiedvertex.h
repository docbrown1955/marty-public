/*!
 * @file simplifiedvertex.h
 *
 * @brief Contains the main interface to obtain simplified vertices, in
 * mty::experimental::ufo::Vertex objects, from a MARTY model.
 *
 * @details Simplified vertices can be obtained in a very simple way using this
 * package. From a MARTY model `model`, it is possible to derive the simple
 * vertices simply typing:
 * \code
 *    std::vector<mty::experimental::ufo::Vertex> vertices
 *        = mty::experimental::ufo::obtainSimplifiedVertices(model);
 * \endcode
 * Once the vertices are obtained, it is possible to loop over them using the
 * `std::vector` features.
 *
 * In case the SM conventions of the model, required to generate a UFO output,
 * are not recognized by the mty::experimental::ufo::obtainSimplifiedVertices() method, it is
 * possible to specify custom conventions using the mty::experimental::ufo::SMData class.
 * By defining an object of type SMData, filling the conventions for SM objects
 * and giving it to the main method, the SM-like features of the model will be
 * recognized and the UFO formatting successful. For example, if the model in
 * use defines an electromagnetic constant `cc_em` that is not standard, one can
 * define it with:
 * \code
 *    SMData smData;
 *    smData.emCouplingName = "cc_em";
 * \endcode
 * Then, the main method can be called giving the `smData` object as a
 * parameter:
 * \code
 *    auto vertices = mty::experimental::ufo::obtainSimplifiedVertices(model, smData);
 * \endcode
 *  We used above the `auto` type deduction.
 *
 * @sa mty::experimental::ufo::Vertex, mty::experimental::ufo::SMData, mty::experimental::ufo::obtainSimplifiedVertices()
 */
#pragma once

#include "ufosmdata.h"
#include "vertexinfo.h"
#include <marty.h>

namespace mty::experimental::ufo
{

    /////////////////////////////////////////////////////
    // Helper struct used as parameter in heper functions
    /////////////////////////////////////////////////////

    /*!
     * @brief Messenger structure passed between functions to carry all the
     * (meta-)data on vertices required to build Vertex objects.
     *
     * @note A user should not have to use this object.
     */
    struct VertexMessenger
    {
        Vertex vertex;                // Vertex modified during the calls
        mty::FeynmanRule const &rule; // Feynman rule that is translated
        mty::Model &model;            // Parent model
        SMData const &smData;         // SM data for conventions

        std::vector<csl::Space const *> colorSpaces = {}; // list of color spaces
    };

    /////////////////////////////////////////////////////
    // Main functions
    /////////////////////////////////////////////////////

    /*!
     * @brief Main interface function to obtain simplified vertices from a MARTY
     * model.
     *
     * @details This function parses the model's Feynman rules and generates the
     * corresponding simplified vertices. The number of vertices is equal to or
     * higher than the number of Feynman rules. The naming conventions for the
     * particular model can be given in the **smData** parameter (optional).
     * Otherwise, default conventions are used.
     *
     * @param model  MARTY model for which simplified vertices must be created.
     * @param smData SM data containing in particular naming conventions for
     * the color group, particle names, coupling names etc.
     *
     * @note There can be more output vertices than input ones when vertices mix
     * Lorentz and color structures (such as the 4-gluon vertex). In this case,
     * the vertex must be expanded and stored into several independent vertices
     * such as what is done in UFO output.
     *
     * @sa Vertex, SMData, simplifiedvertex.h, convertFeynmanRule(),
     * convertFeynmanRules()
     */
    std::vector<Vertex> obtainSimplifiedVertices(
        mty::Model &model,
        SMData const &smData = {});

    /*!
     * @brief Similar to obtainSimplifiedVertices() by giving only one Feynman
     * rule to convert.
     *
     * @param rule   Single Feynman rule to convert to simplified vertices.
     * @param model  MARTY model for which simplified vertices must be created.
     * @param smData SM data containing in particular naming conventions for
     * the color group, particle names, coupling names etc.
     *
     * @sa convertFeynmanRules(), obtainSimplifiedVertices()
     */
    std::vector<Vertex> convertFeynmanRule(
        mty::FeynmanRule const &rule,
        mty::Model &model,
        SMData const &smData);

    /*!
     * @brief Similar to obtainSimplifiedVertices() by giving only a (sub-)set
     * of Feynman rules to convert.
     *
     * @param rules  Set of Feynman rules to convert to simplified vertices.
     * @param model  MARTY model for which simplified vertices must be created.
     * @param smData SM data containing in particular naming conventions for
     * the color group, particle names, coupling names etc.
     *
     * @sa convertFeynmanRule(), obtainSimplifiedVertices()
     */
    std::vector<Vertex> convertFeynmanRules(
        std::vector<mty::FeynmanRule> const &rules,
        mty::Model &model,
        SMData const &smData);

} // namespace mty::experimental::ufo
