#include "InputNumber.h"

// TestWidget::TestWidget ( GUI* rootGUIObject, BaseWidget* parent ) : BaseWidget(rootGUIObject, parent)
// {
// //     m_background = new WidgetBackgroundColored();
// //     BackgroundStyle b;
// //     m_background->init(b, rootGUIObject->BackgroundColoredShader);
//
//     TextWidget* textTest = new TextWidget(rootGUIObject, this);
//     textTest->setText(U"Zeile 1");
//     m_children.push_back(textTest);
//     textTest = new TextWidget(rootGUIObject, this);
//     textTest->changePosition(0, 50);
//     textTest->setText(U"und Zeile 2.");
//     m_children.push_back(textTest);
//
//     //TODO: size reporting for text changes
//     m_width = 200;
//     m_height = 100;
// }
// void TestWidget::onClick ( CForge::Mouse* mouse )
// {
//     printf("reached here");
//     changePosition(mouse->movement()[0],mouse->movement()[1]);
//     for (auto x : m_children) {
//         x->changePosition(mouse->movement()[0],mouse->movement()[1]);
//     }
// }

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
    m_pDec->changePosition(dx, dy);
    m_pValue->changePosition(dx, dy);
    m_pInc->changePosition(dx, dy);
}
void InputNumberWidget::setPosition(float x, float y)
{
    float dx = x - m_x;
    float dy = y - m_y;
    m_x = x;
    m_y = y;
    changePosition(dx, dy);
}
void InputNumberWidget::draw(CForge::RenderDevice* renderDevice)
{
    m_pDec->draw(renderDevice);
    m_pValue->draw(renderDevice);
    m_pInc->draw(renderDevice);
}



TestWidget::TestWidget ( GUI* rootGUIObject, BaseWidget* parent ) : BaseWidget(rootGUIObject, parent)
{
//     m_background = new WidgetBackgroundColored();
//     BackgroundStyle b;
//     m_background->init(b, rootGUIObject->BackgroundColoredShader);

    InputNumberWidget* inputTest = new InputNumberWidget(rootGUIObject, this);
    m_children.push_back(inputTest);

    m_width = inputTest->getWidth();
    m_height = inputTest->getHeight();
}
void TestWidget::onDrag ( mouseEventInfo mouse )
{
    setPosition(mouse.adjustedPosition[0], mouse.adjustedPosition[1]);
    for (auto x : m_children) {
        x->setPosition(mouse.adjustedPosition[0], mouse.adjustedPosition[1]);
    }
}
