/*****************************************************************************\
*                                                                           *
* File(s): InputText.h and InputText.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Simon Kretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTTEXT_H__
#define __CFORGE_INPUTTEXT_H__

#include "../Widget.h"
#include "../GUI.h"

/* IDEA:
The widget could be refactored to be split into an inheritable
base class and the specific text input class seen here. That would
mostly amount to splitting out \ref m_value (and related code) and
\ref m_limits. Would allow for more minimal adaptations to other
inputs (see also CForge::InputSliderWidget_Text, currently is
keeping the string value variables around despite them being
unused). */

namespace CForge {
    class InputTextWidget;

    /**
     * \brief Button to clear all of the text input's entered text.
     * \ingroup GUI
     */
    class CFORGE_API InputTextWidget_ClearButton : public TextWidget {
    public:
        InputTextWidget_ClearButton(GUI* rootGUIObject, InputTextWidget* parent);
        ~InputTextWidget_ClearButton();

        /**
         * \brief Mouse click callback handler.
         *
         * Used to clear the parent text input's entered text.
         *
         * \sa InputTextWidget::clearText
         */
        void onClick(mouseEventInfo) override;

    private:
        InputTextWidget* m_text;    ///< the parent text input widget
    };

    /**
     * \brief An expandable text input widget.
     *
     * Input widget for generic text with replacible validation function.
     * This allows the widget to be reused in other widgets, like for
     * example the CForge::InputSliderWidget for float input.
     *
     * \ingroup GUI
     */
    class InputTextWidget : public BaseWidget {
    public:
        InputTextWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputTextWidget();

        /**
         * \brief Gets the widget's value.
         */
        std::u32string getValue();
        /**
         * \brief Limit the allowed string length.
         *
         * The limits are applied in InputTextWidget::validateInput. As such,
         * custom validation functions can use the limit values for other
         * purposes.
         *
         * If an entered string doesn't comply to the limits, the widget's
         * value will not be changed and the entered text will be highlighted
         * accordingly.
         *
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(int lower, int higher);
        /**
         * \brief Clears all entered text.
         */
        void clearText();
        /**
         * \brief Sets the input widget's value to the given text.
         * \param[in] text      The new text value.
         */
        void setText(std::u32string text);

        //     void onClick(mouseEventInfo mouse) override;
        /**
         * \brief Receive user text input.
         *
         * Inputs apart from Enter/Return are passed on to the CForge::TextWidget
         * used to display the entered text. Once either Enter is pressed
         * or the user clicks away from the text widget, the text widget's
         * current text is requested and run through \ref validateInput.
         * If it is accepted as valid, it is in turn written into \ref m_value.
         * Otherwise, it is higlighted as invalid and m_value remains
         * unmodified at the last valid value.
         *
         * As mentioned elsewhere, registering for any kind of event also
         * enables receiving of focus callbacks. Those are used for color
         * highlighting to indicate input is being accepted.
         *
         * \param[in] c         Entered character.
         */
        void onKeyPress(char32_t c) override;
        /**@{*/
        /**
         * \brief Used for color highlighting.
         */
        void focus() override;
        void focusLost() override;
        /**@}*/

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    protected:
        /**
         * \brief Check if the entered text is valid.
         *
         * By default checks if the length complies with the set limits.
         * However, it can be overwritten to do any other checks.
         * For example, in CForge::InputSliderWidget_Text it is overwritten
         * to check if the input can be parsed as float value and if so,
         * sets its parent CForge::InputSliderWidget's value accordingly.
         */
        virtual bool validateInput();
        std::u32string m_value;     ///< The widget's (last) valid input or `U""` if no valide input was ever entered.
        TextWidget* m_text;         ///< Reference to the text widget used to display the inputs.
    private:
        /**
         * \brief Recompute the widget's layout information.
         *
         * Since text input changes the width of the widget, this function
         * includes the necessary logic to compute the new total width
         * and height and move the InputTextWidget_ClearButton to its
         * proper new location on-screen.
         */
        void recalculateLayout();

        /// Struct containing the set limitations.
        struct {
            int min;                ///< The lower limit.
            int max;                ///< The upper limit.
        } m_limits;                 ///< Currently applied limitations.
        InputTextWidget_ClearButton* m_clear;   ///< Reference to the clear button.
    };

}//name space
#endif
