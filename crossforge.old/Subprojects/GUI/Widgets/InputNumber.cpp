#include "InputNumber.h"
#include "../GUIDefaults.h"
#include <limits>

namespace CForge {

    InputNumberWidget_DecreaseButton::InputNumberWidget_DecreaseButton(GUI* rootGUIObject, InputNumberWidget* parent) : TextWidget(rootGUIObject, parent)
    {
        //need to save the parent in an extra variable because we need access
        //to the value functions not part of the BaseWidget interface.
        m_parentInput = parent;
        setText(U"-");
    }
    void InputNumberWidget_DecreaseButton::onClick(mouseEventInfo)
    {
        m_parentInput->changeValue(-1);
    }
    InputNumberWidget_IncreaseButton::InputNumberWidget_IncreaseButton(GUI* rootGUIObject, InputNumberWidget* parent) : TextWidget(rootGUIObject, parent)
    {
        m_parentInput = parent;
        setText(U"+");
    }
    void InputNumberWidget_IncreaseButton::onClick(mouseEventInfo)
    {
        m_parentInput->changeValue(1);
    }


    InputNumberWidget::InputNumberWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        WidgetStyle defaults;
        m_pValue = new TextWidget(rootGUIObject, this);
        m_pInc = new InputNumberWidget_IncreaseButton(rootGUIObject, this);
        m_pDec = new InputNumberWidget_DecreaseButton(rootGUIObject, this);
        m_pValue->changePosition(m_pDec->getWidth() + defaults.WithinWidgetPadding, 0);
        m_value = 0;
        m_pValue->setText(U"0");
        m_pInc->changePosition(m_pDec->getWidth() + m_pValue->getWidth() + 2 * defaults.WithinWidgetPadding, 0);

        rootGUIObject->registerMouseDownEvent(m_pDec);
        rootGUIObject->registerMouseDownEvent(m_pInc);
        rootGUIObject->registerKeyPressEvent(this);

        m_height = m_pValue->getHeight();
        m_width = m_pDec->getWidth() + m_pValue->getWidth() + m_pInc->getWidth() + 2 * defaults.WithinWidgetPadding;
        m_negativeInput = false;
        m_limits.min = std::numeric_limits<int>::min();
        m_limits.max = std::numeric_limits<int>::max();

        m_background = new WidgetBackgroundBorder(this, m_root);
        //     m_background->setColor(1, 1, 1, 0.1);
    }
    InputNumberWidget::~InputNumberWidget()
    {
        delete m_pValue;
        delete m_pDec;
        delete m_pInc;
        delete m_background;
    }

    int InputNumberWidget::getValue()
    {
        return m_value;
    }
    void InputNumberWidget::setValue(int value)
    {
        m_value = value;
        //The value's display width will probably change.
        float positionAdjust = -m_pValue->getWidth();
        //u32strings don't have a to_string() method of their own, unfortunately.
        //However, wstrings on most systems should be compatible with Unicode or
        //at the very least ASCII, where most characters used in numbers are at.
        std::wstring valueString = std::to_wstring(m_value);
        std::u32string u32Value;
        for (char x : valueString) {
            u32Value.push_back((char32_t)x);
        }
        m_pValue->setText(u32Value);
        //calculate m_pInc's new position with the value's changed width
        positionAdjust += m_pValue->getWidth();
        m_pInc->changePosition(positionAdjust, 0);
        updateLayout();

        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }
    void InputNumberWidget::changeValue(int value)
    {
        long newValue = (long)m_value + value;
        if (newValue <= m_limits.max && newValue >= m_limits.min)
            setValue(newValue);
    }
    void InputNumberWidget::setLimit(int lower, int higher)
    {
        m_limits.min = lower;
        m_limits.max = higher;

        if (m_value < m_limits.min) setValue(m_limits.min);
        else if (m_value > m_limits.max) setValue(m_limits.max);
    }
    void InputNumberWidget::focus()
    {
        FontStyle1 f;
        m_pValue->setColor(f.FontColorHighlight);
    }
    void InputNumberWidget::focusLost()
    {
        FontStyle1 f;
        m_pValue->setColor(f.FontColor);
    }
    void InputNumberWidget::onKeyPress(char32_t character)
    {
        if (character == U'\b') {
            //delete the last number by using integer division
            setValue(m_value / 10);
            return;
        }
        if (character == U'-') {
            //next input should be negative if it's currently 0
            if (m_value == 0) m_negativeInput = true;
            return;
        }
        //add to the end of number
        if (character > 0x39 || character < 0x30) return;
        int enteredNumber = character - 0x30;
        //prevent integer overflow by using a long for the limits check
        long newValue = m_value;
        if (newValue == 0 && m_negativeInput) {
            enteredNumber *= -1;
            m_negativeInput = false;
        }
        if (newValue >= 0) {
            newValue = newValue * 10 + enteredNumber;
        }
        else {
            newValue = newValue * 10 - enteredNumber;
        }
        if (newValue <= m_limits.max && newValue >= m_limits.min)
            setValue(newValue);
        return;
    }

    void InputNumberWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        m_pDec->changePosition(dx, dy);
        m_pValue->changePosition(dx, dy);
        m_pInc->changePosition(dx, dy);
        m_background->updatePosition();
    }
    void InputNumberWidget::updateLayout()
    {
        WidgetStyle defaults;
        m_height = m_pValue->getHeight();
        m_width = m_pDec->getWidth() + m_pValue->getWidth() + m_pInc->getWidth() + 2 * defaults.WithinWidgetPadding;

        m_background->updateSize();

        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void InputNumberWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_pDec->draw(renderDevice);
        m_pValue->draw(renderDevice);
        m_pInc->draw(renderDevice);
    }

}//name space