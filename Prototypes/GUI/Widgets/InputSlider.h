/*****************************************************************************\
*                                                                           *
* File(s): InputSlider.h and InputSlider.cpp                                      *
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
#ifndef __CFORGE_INPUTSLIDER_H__
#define __CFORGE_INPUTSLIDER_H__

#include "../Widget.h"
#include "../GUI.h"
#include "InputText.h"

namespace CForge {

    class InputSliderWidget;

    /**
     * \brief Widget for slider input for the indicator of the current value on the slider.
     *
     * Created by InputSliderWidget_Slider. No actual functionality, only
     * moved around by the former to indicate the current value.
     *
     * \ingroup GUI
     */
    class InputSliderWidget_SliderCursor : public BaseWidget {
    public:
        InputSliderWidget_SliderCursor(GUI* rootGUIObject, BaseWidget* parent);
        ~InputSliderWidget_SliderCursor();
    };

    /**
     * \brief Widget for slider input for the actual slider part of this input widget.
     * \ingroup GUI
     */
    class CFORGE_API InputSliderWidget_Slider : public BaseWidget {
    public:
        InputSliderWidget_Slider(GUI* rootGUIObject, InputSliderWidget* parent);
        ~InputSliderWidget_Slider();

        /**
         * \brief Set the slider's value indicator's position.
         * \param[in] percantage Percantage of the slider. 0% is the slider's
         *                       lowest value, 100% is the maximum value.
         */
        void setCursorPosition(float percantage);

        /**
         * \brief Mouse click callback handler.
         *
         * Used to change the input widget's value using the slider.
         * Calculates the position of the mouse pointer within the slider
         * as percantage of the slider length and passes it to
         * InputSliderWidget::setValueByPercentage.
         *
         * \param[in] mouse     Mouse cursor position.
         */
        void onClick(mouseEventInfo mouse) override;

        /**
         * \brief Mouse drag callback handler.
         *
         * Used to change the input widget's value using the slider.
         * \sa InputSliderWidget_Slider::onClick InputSliderWidget::setValueByPercentage
         *
         * \param[in] mouse     Mouse cursor position.
         */
        void onDrag(mouseEventInfo mouse) override;

        void changePosition(float dx, float dy) override;
        //     void updateLayout() override;        //we won't change the slider's dimensions
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        InputSliderWidget* m_slider;                ///< Reference to the parent slider widget.
        InputSliderWidget_SliderCursor* m_cursor;   ///< The value indicator on the slider.
    };

    /**
     * \brief Widget for slider input for textual display and direct entry of the value.
     *
     * Inherits a large part of its functionality from the
     * CForge::InputTextWidget.
     *
     * \ingroup GUI
     */
    class InputSliderWidget_Text : public InputTextWidget {
    public:
        InputSliderWidget_Text(GUI* rootGUIObject, InputSliderWidget* parent);
        ~InputSliderWidget_Text();

        /**
         * \brief Returns the entered text as float value.
         *
         * As this sub-widget inherits from CForge::InputTextWidget, the same
         * rules for updating the value as mentioned in
         * CForge::InputTextWidget::onKeyPress apply. Namely, this function will
         * only return the last *valid* input, which may not be what is currently
         * entered.
         */
        float getFloatValue();

        /**
         * \brief Sets the value and displayed text to the one passed in.
         *
         * Internally converts the value to a rounded representation with the
         * amount of decimals specified in CForge::InputSliderStyle#RoundLabelNumber.
         * Note that only the visuals are rounded, the float value is set as is.
         *
         * \param[in] value     New value to be shown.
         */
        void setFloatValue(float value);

    private:

        /**
         * \brief Checks if input can be read as float and sets value if it does.
         *
         * This validation function tries to parse the given input as float value
         * using std::stof. If it succeeds, the value is then submitted to
         * InputSliderWidget::setValue to be used as new value of the widget.
         *
         * One thing to note might be that this function here doesn't actually
         * set the value of this here text widget. That is instead left to
         * InputSliderWidget::setValue as only that function performs the limit
         * and step size checks. It will then call \ref setFloatValue.
         *
         * \param[in] value     New value to be shown.
         */
        bool validateInput() override;

        float m_floatValue;             ///< The (last valid) input parsed as float value.
        InputSliderWidget* m_slider;    ///< Reference to the parent slider input widget.
    };

    /**
     * \brief Input widget for entering float numbers using a slider and/or text.
     *
     * Implements input method commonly referred to as slider. A value range
     * is represented by a bar and an indicator on that bar shows the current
     * value. This indicator can be moved by clicking it with the mouse. It is
     * thus one of the more interactive widgets.
     *
     * In addition to this slider, this widget also adds a text input field
     * to allow direct entering of values and provide a way to display the
     * exact value it is currently set to.
     *
     * \ingroup GUI
     */
    class InputSliderWidget : public BaseWidget {
    public:
        InputSliderWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputSliderWidget();

        /**
         * \brief Returns the currently set value.
         */
        float getValue();

        /**
         * \brief Sets the given value and updates the display of all related sub-widgets.
         *
         * This function is usually called from text input's
         * InputSliderWidget_Text::validateInput method but can also be called
         * manually or through CForge::FormWidget::setDefault.
         *
         * Does a number of things. Firstly it processes the given value in
         * accordance to the set value range limits and step size. That
         * adapted value is then saved as this widget's value and lastly all
         * the subwidgets (InputSliderWidget_Slider, InputSliderWidget_Text)
         * are updated with that new value.
         *
         * \param[in] value     New value to be shown.
         * \sa InputSliderWidget_Slider::setCursorPosition InputSliderWidget_Text::setFloatValue
         */
        void setValue(float value);

        /**
         * \brief Sets the given value using a slider percantage and updates the display of all related sub-widgets.
         *
         * Just a wrapper around \ref setValue to work with the slider
         * percantages used by InputSliderWidget_Slider.
         *
         * \param[in] sliderPercantage Percantage of the value range the slider was set to.
         * \sa InputSliderWidget_Slider::onClick
         */
        void setValueByPercentage(float sliderPercantage);

        /**
         * \brief Set the slider's value range.
         *
         * Integer overload function for convenience.
         * \sa setLimit(float lower, float higher)
         *
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(int lower, int higher);

        /**
         * \brief Set the slider's value range.
         *
         * Defines the value range over which the slider operates. If not
         * explicitly called, a default of 0 to 10 is used.
         *
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(float lower, float higher);

        /**
         * \brief Set the slider's step size.
         *
         * Defines in what steps the slider can be moved. If the value range
         * is not an exact multiple of the step size, the last step might be
         * smaller leading to the maximum allowed value.
         *
         * If not set, the default of 1 will be used.
         *
         * \param[in] stepSize  Value delta between two stops on the slider.
         */
        void setStepSize(float stepSize);

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        float m_value;      ///< The input widget's current value.
        struct {
            float min;
            float max;
        } m_limits;         ///< The current value range. (0 to 10 by default.)
        float m_stepSize;   ///< The current step size. Defaults to 1.

        InputSliderWidget_Text* m_text;     ///< Reference to the text input.
        InputSliderWidget_Slider* m_slide;  ///< Reference to the widget for the slider part of the input.
    };

}//name space
#endif
