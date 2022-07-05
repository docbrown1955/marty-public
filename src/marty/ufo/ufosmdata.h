/*!
 * @file ufosmdata.h
 *
 * @brief Contains the mty::experimental::ufo::SMData class.
 */
#pragma once

#include <string>

namespace mty::experimental::ufo
{

    /*!
     * @brief Class allowing to define custom conventions for SM-like models.
     *
     * @details All the SM conventions required for the generation of UFO files
     * should be configurable by using a SMData object, and all configuration
     * should have a default value. This class allows the conversion algorithms
     * (from MARTY to UFO) to be convention-blind. Instead of defining fixed
     * conventions for e.g. the photon or electromagnetic coupling names, all
     * the algorithms use a SMData that should contain the conventions used in
     * the MARTY model that is processed to generate a UFO output.
     *
     * For example, in a model in which the color group is named `"SU3c"`
     * instead of `"C"`, it is possible to generate the UFO output by creating
     * a custom SMData object and specifying the color group name:
     * \code
     *    mty::Model model = someModel();
     *    SMData smData;
     *    smData.colorName = "SU3c";
     *    auto vertices = ufo::obtainSimplifiedVertices(model, smData);
     * \endcode
     */
    struct SMData
    {

        /*!
         * @brief Name of the SM color group.
         */
        std::string colorName = "C";

        /*!
         * @brief Name of the photon particle.
         */
        std::string photonName = "A";

        /*!
         * @brief Name of the electromagnetic coupling constant.
         */
        std::string emCouplingName = "e_em";
    };

} // namespace mty::experimental::ufo
