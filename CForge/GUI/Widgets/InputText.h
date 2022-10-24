/*****************************************************************************\
*                                                                           *
* File(s): InputText.h and InputText.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTTEXT_H__
#define __CFORGE_INPUTTEXT_H__

#include "../Widget.h"
#include "../GUI.h"

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
    class CFORGE_API InputTextWidget : public BaseWidget {
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
         */
        virtual bool validateInput();
        std::u32string m_value;
        TextWidget* m_text;
    private:
        void recalculateLayout();

        struct {
            int min;
            int max;
        } m_limits;
        InputTextWidget_ClearButton* m_clear;
    };

}//name space
#endif
