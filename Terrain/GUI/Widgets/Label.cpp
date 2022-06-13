/*
The LabelWidget is meant to abstract away some of the differences
between the various kinds of input widgets, specifically cramming
all the different value types into a single data type.

This is achieved through some rather ugly pointer casting and a
lot of switch statements.

The input's value is mirrored inside of this widget as base for
the getValue's data pointer, so that the original input's value
can't (accidentally) be tampered with by some callback's code.
 */

#include "Label.h"

LabelWidget::LabelWidget(CallbackDatatype type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
{
    m_type = type;
    m_pLabelText = new TextWidget(rootGUIObject, this);
    m_pLabelText->setText(labelText);
    switch (m_type) {
        case DATATYPE_INT:
            m_pValue = new int;
            *(int*)m_pValue = 0;
            m_pInput = new InputNumberWidget(rootGUIObject, this);
            break;
        case DATATYPE_BOOLEAN:
            m_pValue = new bool;
            *(bool*)m_pValue = false;
            m_pInput = new InputCheckboxWidget(rootGUIObject, this);
            break;
        case DATATYPE_STRING:
            m_pValue = new std::u32string;
            *(std::u32string*)m_pValue = U"";
            m_pInput = new InputTextWidget(rootGUIObject, this);
            break;
        default:
            //invalid type or no matching input yet
            m_pInput = nullptr;
            break;
    }
    m_width = m_pLabelText->getWidth();
    m_height = m_pLabelText->getHeight();
    if (m_pInput) {
        WidgetStyle defaults;
        m_justification = m_pLabelText->getWidth() + defaults.WithinWidgetPadding;
        m_pInput->changePosition(m_justification, 0);
        m_width = m_justification + m_pInput->getWidth();
        m_height = std::max(m_pInput->getHeight(), m_height);
    }
}
LabelWidget::~LabelWidget()
{
    delete m_pLabelText;
    delete m_pInput;
    switch (m_type) {
        case DATATYPE_INT:
            delete (int*)m_pValue;
            break;
        case DATATYPE_BOOLEAN:
            delete (bool*)m_pValue;
            break;
        case DATATYPE_STRING:
            delete (std::u32string*)m_pValue;
            break;
        default:
            //invalid type or no matching input yet
            break;
    }
}

CallbackDatum LabelWidget::getValue()
{
    switch (m_type) {
        case DATATYPE_INT:
            *(int*)m_pValue = (*(InputNumberWidget*)m_pInput).getValue();
            break;
        case DATATYPE_BOOLEAN:
            *(bool*)m_pValue = (*(InputCheckboxWidget*)m_pInput).getValue();
            break;
        case DATATYPE_STRING:
            *(std::u32string*)m_pValue = (*(InputTextWidget*)m_pInput).getValue();
            break;
        default:
            //invalid type or no matching input yet
            break;
    }

    CallbackDatum retObj;
    retObj.Type = m_type;
    retObj.pData = m_pValue;
    return retObj;
}
void LabelWidget::setLimit(int lower, int higher)
{
    switch (m_type) {
        case DATATYPE_INT:
            (*(InputNumberWidget*)m_pInput).setLimit(lower, higher);
            break;
        case DATATYPE_BOOLEAN:
            //A boolean value cannot be limited in range
            break;
        case DATATYPE_STRING:
            (*(InputTextWidget*)m_pInput).setLimit(lower, higher);
            break;
        default:
            //invalid type or no matching input yet
            break;
    }
}
void LabelWidget::setDefault ( int value )
{
    if (m_type == DATATYPE_INT) {
        (*(InputNumberWidget*)m_pInput).setValue(value);
    }
}
void LabelWidget::setDefault ( bool value )
{
    if (m_type == DATATYPE_BOOLEAN) {
        (*(InputCheckboxWidget*)m_pInput).setState(value);
    }
}


float LabelWidget::getJustification()
{
    return m_justification;
}
void LabelWidget::setJustification(float j)
{
    if (m_pInput) {
        m_width = m_pLabelText->getWidth();
        m_height = m_pLabelText->getHeight();
        m_justification = j;
        m_pInput->setPosition(m_x + m_justification, m_y);
        m_width = m_justification + m_pInput->getWidth();
        m_height = std::max(m_pInput->getHeight(), m_height);
    }
}

void LabelWidget::changePosition(float dx, float dy)
{
    m_x += dx;
    m_y += dy;
    m_pLabelText->changePosition(dx, dy);
    m_pInput->changePosition(dx, dy);
}
void LabelWidget::updateLayout()
{
    m_width = m_pLabelText->getWidth();
    m_height = m_pLabelText->getHeight();
    if (m_pInput) {
        m_width += 30 + m_pInput->getWidth();
        m_height = std::max(m_pInput->getHeight(), m_height);
    }
    if (m_parent != nullptr) m_parent->updateLayout();
}
void LabelWidget::draw(CForge::RenderDevice* renderDevice)
{
    m_pLabelText->draw(renderDevice);
    m_pInput->draw(renderDevice);
}

