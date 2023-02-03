#include "InputSlider.h"

namespace CForge {

    InputSliderWidget_SliderCursor::InputSliderWidget_SliderCursor(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        InputSliderStyle defaults;
        m_width = defaults.CursorWidth;
        m_height = defaults.Height;
        m_background = new WidgetBackgroundBorder(this, m_root);
        //TODO: should (and hopefully will) be updated to not rely on this silly pointer cast
        (*(WidgetBackgroundBorder*)m_background).setLineWidth(defaults.CursorBorder);
    }
    InputSliderWidget_SliderCursor::~InputSliderWidget_SliderCursor()
    {
        delete m_background;
    }


    InputSliderWidget_Slider::InputSliderWidget_Slider(GUI* rootGUIObject, InputSliderWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_slider = parent;

        InputSliderStyle defaults;
        m_width = defaults.Width;
        m_height = defaults.Height;
        m_background = new WidgetBackgroundBorder(this, m_root);

        m_cursor = new InputSliderWidget_SliderCursor(rootGUIObject, this);

        m_root->registerMouseDownEvent(this);
        m_root->registerMouseDragEvent(this);
    }
    InputSliderWidget_Slider::~InputSliderWidget_Slider()
    {
        delete m_background;
        delete m_cursor;
    }
    void InputSliderWidget_Slider::onClick(mouseEventInfo mouse)
    {
        float percentage = (mouse.rawPosition.x() - m_x) / m_width;
        m_slider->setValueByPercentage(percentage);
    }
    void InputSliderWidget_Slider::onDrag(mouseEventInfo mouse)
    {
        float percentage = (mouse.rawPosition.x() - m_x) / m_width;
        m_slider->setValueByPercentage(percentage);
    }
    void InputSliderWidget_Slider::setCursorPosition(float percantage)
    {
        m_cursor->setPosition(m_x + percantage * m_width, m_y);
    }
    void InputSliderWidget_Slider::changePosition(float dx, float dy)
    {
        BaseWidget::changePosition(dx, dy);
        m_cursor->changePosition(dx, dy);
    }
    void InputSliderWidget_Slider::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_cursor->draw(renderDevice);
    }


    InputSliderWidget_Text::InputSliderWidget_Text(GUI* rootGUIObject, InputSliderWidget* parent) : InputTextWidget(rootGUIObject, parent)
    {
        m_slider = parent;
    }
    InputSliderWidget_Text::~InputSliderWidget_Text()
    {
        //Do we need this?
    }
    float InputSliderWidget_Text::getFloatValue()
    {
        return m_floatValue;
    }
    void InputSliderWidget_Text::setFloatValue(float value)
    {
        //Just in case this gets triggered accidentally somewhere, avoid unnecessary work
        if (value != m_floatValue) {
            m_floatValue = value;
            //ugly cast to u32string
            InputSliderStyle defaults;
            std::u32string stringValue;
            if (defaults.RoundLabelNumber >= 0) {
                int displayValue = std::round(value * std::pow(10, defaults.RoundLabelNumber));
                std::wstring stringRep = std::to_wstring(displayValue);
                if (stringRep.length() < defaults.RoundLabelNumber + 1) {
                    stringRep.insert(0, defaults.RoundLabelNumber - stringRep.length() + 1, L'0');
                }
                for (int i = 0; i < stringRep.length(); i++) {
                    if (i == stringRep.length() - defaults.RoundLabelNumber && defaults.RoundLabelNumber != 0) {
                        stringValue.push_back(U'.');
                    }
                    stringValue.push_back((char32_t)stringRep[i]);
                }
            }
            else {
                for (auto x : std::to_wstring(m_floatValue)) {
                    stringValue.push_back((char32_t)x);
                }
            }
            setText(stringValue);
            m_value = stringValue;        //not actually used for much other than return value, but can't hurt to set it
            //need to reset font color after bad input is replaced
            FontStyle1 textdefaults;
            m_text->setColor(textdefaults.FontColor);
        }
    }
    bool InputSliderWidget_Text::validateInput()
    {
        //again, ugly cast to wstring this time
        std::wstring w;
        for (auto c : m_text->getText()) {
            w.push_back((wchar_t)c);
        };
        try {
            //we write it directly to m_floatValue so that setFloatValue actually does things
            //unless it's really exactly the same value
            m_floatValue = std::stof(w);
            //we let the "main" slider class handle making sense of the float.
            //this should call setFloatValue, thus doing everything that would need to be done here
            m_slider->setValue(m_floatValue);
            return true;
        }
        catch (...) {
            return false;
        }
    }


    InputSliderWidget::InputSliderWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_text = new InputSliderWidget_Text(m_root, this);
        m_slide = new InputSliderWidget_Slider(m_root, this);

        WidgetStyle defaults;
        m_text->changePosition(m_slide->getWidth() + defaults.WithinWidgetPadding, 0);
        if (m_text->getHeight() > m_slide->getHeight()) {
            float heightAdjust = 0.5 * (m_text->getHeight() - m_slide->getHeight());
            m_slide->changePosition(0, heightAdjust);
            m_height = m_text->getHeight();
        }
        else {
            float heightAdjust = 0.5 * (m_slide->getHeight() - m_text->getHeight());
            m_text->changePosition(0, heightAdjust);
            m_height = m_slide->getHeight();
        }

        m_limits.min = 0;   //some default values
        m_limits.max = 10;
        m_stepSize = 1;

        m_width = m_slide->getWidth() + defaults.WithinWidgetPadding + m_text->getWidth();
    }
    InputSliderWidget::~InputSliderWidget()
    {
        delete m_text;
        delete m_slide;
    }

    float InputSliderWidget::getValue()
    {
        return m_value;
    }
    void InputSliderWidget::setValue(float value)
    {
        //most likely the result of someone entering it in the text field
        //we still need to make sense of this value, as it's not aligned to the stepsize yet

        //handle the stepsize
        float newValue = std::round(value / m_stepSize) * m_stepSize;
        //clamp to value range
        if (newValue < m_limits.min) newValue = m_limits.min;
        if (newValue > m_limits.max) newValue = m_limits.max;

        //apply the adjusted Value
        m_value = newValue;
        m_text->setFloatValue(newValue);
        float percantage = (newValue - m_limits.min) / (m_limits.max - m_limits.min);
        m_slide->setCursorPosition(percantage);

        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }
    void InputSliderWidget::setValueByPercentage(float sliderPercantage)
    {
        setValue(m_limits.min + sliderPercantage * (m_limits.max - m_limits.min));
    }
    void InputSliderWidget::setLimit(int lower, int higher)
    {
        m_limits.min = lower;
        m_limits.max = higher;
    }
    void InputSliderWidget::setLimit(float lower, float higher)
    {
        m_limits.min = lower;
        m_limits.max = higher;
    }
    void InputSliderWidget::setStepSize(float stepSize)
    {
        m_stepSize = stepSize;
    }
    void InputSliderWidget::changePosition(float dx, float dy)
    {
        BaseWidget::changePosition(dx, dy);
        m_text->changePosition(dx, dy);
        m_slide->changePosition(dx, dy);
    }
    void InputSliderWidget::updateLayout()
    {
        //only m_text should change its width, rest should remain as it was
        WidgetStyle defaults;
        m_width = m_slide->getWidth() + defaults.WithinWidgetPadding + m_text->getWidth();
        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void InputSliderWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_slide->draw(renderDevice);
        m_text->draw(renderDevice);
    }


}//name space