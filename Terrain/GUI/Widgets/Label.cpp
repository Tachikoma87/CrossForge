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
        default:
            //invalid type or no matching input yet
            m_pInput = nullptr;
            break;
    }
    m_width = m_pLabelText->getWidth();
    m_height = m_pLabelText->getHeight();
    if (m_pInput) {
        m_pInput->changePosition(m_pLabelText->getWidth() + 30, 0);
        m_width += 30 + m_pInput->getWidth();
        m_height = std::max(m_pInput->getHeight(), m_height);
    }
}
LabelWidget::~LabelWidget()
{
    delete m_pLabelText;
    switch (m_type) {
        case DATATYPE_INT:
            delete (int*)m_pValue;
            delete (InputNumberWidget*)m_pInput;
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
        default:
            //invalid type or no matching input yet
            break;
    }

    CallbackDatum retObj;
    retObj.Type = m_type;
    retObj.pData = m_pValue;
    return retObj;
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

