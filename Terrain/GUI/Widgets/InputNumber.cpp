#include "InputNumber.h"

InputNumberWidget_DecreaseButton::InputNumberWidget_DecreaseButton(GUI* rootGUIObject, InputNumberWidget* parent): TextWidget(rootGUIObject, parent)
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
InputNumberWidget_IncreaseButton::InputNumberWidget_IncreaseButton(GUI* rootGUIObject, InputNumberWidget* parent): TextWidget(rootGUIObject, parent)
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
    m_pInc->changePosition(m_pDec->getWidth() + m_pValue->getWidth() + 2*defaults.WithinWidgetPadding, 0);

    rootGUIObject->registerMouseDownEvent(m_pDec);
    rootGUIObject->registerMouseDownEvent(m_pInc);

    m_height = m_pValue->getHeight();
    m_width = m_pDec->getWidth() + m_pValue->getWidth() + m_pInc->getWidth() + 2*defaults.WithinWidgetPadding;
}
InputNumberWidget::~InputNumberWidget()
{
    delete m_pValue;
    delete m_pDec;
    delete m_pInc;
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
    //However, strings produced by to_string() should in almost all cases
    //stay within ASCII encoding, which unicode is compatible to.
    std::string valueString = std::to_string(m_value);
    std::u32string u32Value;
    for (char x : valueString) {
        u32Value.push_back((char32_t)x);
    }
    m_pValue->setText(u32Value);
    //calculate m_pInc's new position with the value's changed width
    positionAdjust += m_pValue->getWidth();
    m_pInc->changePosition(positionAdjust, 0);
}
void InputNumberWidget::changeValue(int value)
{
    setValue(m_value + value);
}
void InputNumberWidget::changePosition(float dx, float dy)
{
    m_x += dx;
    m_y += dy;
    m_pDec->changePosition(dx, dy);
    m_pValue->changePosition(dx, dy);
    m_pInc->changePosition(dx, dy);
}
void InputNumberWidget::draw(CForge::RenderDevice* renderDevice)
{
    m_pDec->draw(renderDevice);
    m_pValue->draw(renderDevice);
    m_pInc->draw(renderDevice);
}
