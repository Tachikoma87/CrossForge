/*****************************************************************************\
*                                                                           *
* File(s): InputNumber.h and InputNumber.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTNUMBER_H__
#define __CFORGE_INPUTNUMBER_H__

#include "../Widget.h"
#include "../GUI.h"

/*
 IDEA:
    Rewrite the text display/input to use the InputTextWidget akin
    to the InputSliderWidget. Would allow to use better input if
    the InputTextWidget was ever expanded beyond the absolute basics.
    Reason it doesn't already use it, is because the number input
    was created earlier than the text input.
 */

namespace CForge {

    class InputNumberWidget;

    /**
     * \brief Widget to increase the integer number input by one.
     * \ingroup GUI
     */
    class InputNumberWidget_DecreaseButton : public TextWidget {
    public:
        InputNumberWidget_DecreaseButton(GUI* rootGUIObject, InputNumberWidget* parent);

        /**
         * \brief Mouse click callback handler.
         *
         * Used to increase the parent InputNumberWidget's value by 1.
         */
        void onClick(mouseEventInfo) override;
    private:
        InputNumberWidget* m_parentInput;   ///< Reference to the parent InputNumberWidget.
    };

    /**
     * \brief Widget to decrease the integer number input by one.
     * \ingroup GUI
     */
    class InputNumberWidget_IncreaseButton : public TextWidget {
    public:
        InputNumberWidget_IncreaseButton(GUI* rootGUIObject, InputNumberWidget* parent);

        /**
         * \brief Mouse click callback handler.
         *
         * Used to decrease the parent InputNumberWidget's value by 1.
         */
        void onClick(mouseEventInfo) override;
    private:
        InputNumberWidget* m_parentInput;   ///< Reference to the parent InputNumberWidget.
    };

    /**
     * \brief Widget to enter integer numbers.
     *
     * Apart from plus and minus buttons to in-/decrease by one, it also
     * features some basic keyboard input capabilities.
     *
     * \ingroup GUI
     */
    class InputNumberWidget : public BaseWidget {
    public:
        InputNumberWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputNumberWidget();

        /**
         * \brief Sets the value and displayed text to the one passed in.
         *
         * The conversion of the integer to the displayed string is performed
         * by std::to_wstring.
         *
         * \param[in] value     New value to be shown.
         */
        void setValue(int value);

        /**
         * \brief Adds the given value to the current value.
         *
         * Used by the plus/minus buttons to change the value.
         *
         * \param[in] value     Value to add to the current value.
         */
        void changeValue(int value);

        int getValue();         ///< Getter for the widget's value.

        /**
         * \brief Set the allowed value range.
         *
         * Defines the value range of the integer. Default is from
         * std::numeric_limits<int>::min() to std::numeric_limits<int>::max()
         *
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(int lower, int higher);

        /**
         * \brief Used for color highlighting to signal the widget is accepting keyboard input.
         */
        void focus() override;

        /**
         * \brief Used to reset the color to default.
         */
        void focusLost() override;

        /**
         * \brief Receive user text input.
         *
         * Only numeric characters are accepted (`0`-`9`), and are always
         * appended to the end of the number. The last digit can be deleted
         * by pressing backspace. The only other allowed key is `-`,
         * which may be used on an empty number input (value being zero)
         * to have the following input start a negative number.
         *
         * \param[in] c         Entered character.
         */
        void onKeyPress(char32_t character) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        int m_value;            ///< The current value.
        struct {
            int min;
            int max;
        } m_limits;             ///< The allowed value range.
        InputNumberWidget_DecreaseButton* m_pDec;   ///< Reference to "minus" button to decrease the number by one.
        InputNumberWidget_IncreaseButton* m_pInc;   ///< Reference to "plus" button to increase the number by one.
        TextWidget* m_pValue;   ///< Reference to the TextWidget used to display the current value.
        bool m_negativeInput;   ///< Whether the next input should start a negative number. \sa onKeyPress
    };

}//name space
#endif
