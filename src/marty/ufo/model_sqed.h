/*!
 * @file model_sqed.h
 *
 * @brief Template for model definition in MARTY-UFO.
 */
#pragma once

#include <marty.h>

namespace mty::experimental::ufo
{

    /*!
     * @brief Template model showing how to build models to be tested / used in
     * MARTY-UFO.
     *
     * @details The model building must happen in a separate initialize() method
     * and not directly in the constructor. The constructor, depending on the
     * value of a boolean parameter can call (or not) the initialization method.
     */
    class Model_SQED : public mty::Model
    {

    public:
        /*!
         * @brief Constructor.
         *
         * @details If not initilized, the model is empty and not in a valid
         * state. The inititialize() function should be called one way or
         * another before using the model.
         *
         * @param do_initialize Tells if the model must be
         * initialized by calling the initialize() function (default = true).
         */
        Model_SQED(bool do_initialize = true);

        /*!
         * @brief Initializes the gauge and scalar for the model.
         */
        void initialize();
    };

} // Namespace mty::experimental::ufo
