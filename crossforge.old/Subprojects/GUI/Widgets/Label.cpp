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

namespace CForge {

    LabelWidget::LabelWidget(GUIInputType type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_type = type;
        m_pLabelText = new TextWidget(rootGUIObject, this);
        m_pLabelText->setText(labelText);
        switch (m_type) {
        case INPUTTYPE_INT:
            m_pValue = new int;
            *(int*)m_pValue = 0;
            m_pInput = new InputNumberWidget(rootGUIObject, this);
            break;
        case INPUTTYPE_BOOL:
            m_pValue = new bool;
            *(bool*)m_pValue = false;
            m_pInput = new InputCheckboxWidget(rootGUIObject, this);
            break;
        case INPUTTYPE_STRING:
            m_pValue = new std::u32string;
            *(std::u32string*)m_pValue = U"";
            m_pInput = new InputTextWidget(rootGUIObject, this);
            break;
        case INPUTTYPE_RANGESLIDER:
            m_pValue = new float;
            *(float*)m_pValue = 0;
            m_pInput = new InputSliderWidget(rootGUIObject, this);
            break;
        case INPUTTYPE_DROPDOWN:
            m_pValue = new int;
            *(int*)m_pValue = 0;
            m_pInput = new InputDropDownWidget(rootGUIObject, this);
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
        case INPUTTYPE_DROPDOWN:
        case INPUTTYPE_INT:
            delete (int*)m_pValue;
            break;
        case INPUTTYPE_BOOL:
            delete (bool*)m_pValue;
            break;
        case INPUTTYPE_STRING:
            delete (std::u32string*)m_pValue;
            break;
        case INPUTTYPE_RANGESLIDER:
            delete (float*)m_pValue;
            break;
        default:
            //invalid type or no matching input yet
            break;
        }
    }

    GUICallbackDatum LabelWidget::getValue()
    {
        switch (m_type) {
        case INPUTTYPE_INT:
            *(int*)m_pValue = (*(InputNumberWidget*)m_pInput).getValue();
            break;
        case INPUTTYPE_BOOL:
            *(bool*)m_pValue = (*(InputCheckboxWidget*)m_pInput).getValue();
            break;
        case INPUTTYPE_STRING:
            *(std::u32string*)m_pValue = (*(InputTextWidget*)m_pInput).getValue();
            break;
        case INPUTTYPE_RANGESLIDER:
            *(float*)m_pValue = (*(InputSliderWidget*)m_pInput).getValue();
            break;
        case INPUTTYPE_DROPDOWN:
            *(int*)m_pValue = (*(InputDropDownWidget*)m_pInput).getValue();
        default:
            //invalid type or no matching input yet
            break;
        }

        GUICallbackDatum retObj;
        retObj.Type = m_type;
        retObj.pData = m_pValue;
        return retObj;
    }
    void LabelWidget::setLimit(int lower, int higher)
    {
        switch (m_type) {
        case INPUTTYPE_INT:
            (*(InputNumberWidget*)m_pInput).setLimit(lower, higher);
            break;
            /*case INPUTTYPE_BOOL:
                //A boolean value cannot be limited in range
                break;*/
        case INPUTTYPE_STRING:
            (*(InputTextWidget*)m_pInput).setLimit(lower, higher);
            break;
        case INPUTTYPE_RANGESLIDER:
            (*(InputSliderWidget*)m_pInput).setLimit(lower, higher);
            break;
            /*case INPUTTYPE_DROPDOWN:
                //A dropdown is limited by its given options instead
                break;*/
        default:
            //invalid type or no matching input yet
            break;
        }
    }
    void LabelWidget::setLimit(float lower, float higher)
    {
        switch (m_type) {
        case INPUTTYPE_RANGESLIDER:
            (*(InputSliderWidget*)m_pInput).setLimit(lower, higher);
            break;
        default:
            //invalid type or no matching input yet
            break;
        }
    }
    void LabelWidget::setDefault(int value)
    {
        switch (m_type) {
        case INPUTTYPE_INT:
            (*(InputNumberWidget*)m_pInput).setValue(value);
            break;
        case INPUTTYPE_BOOL:
            (*(InputCheckboxWidget*)m_pInput).setState(value != 0);
            break;
            /*case INPUTTYPE_STRING:
                //doesn't make much sense imo
                break;*/
        case INPUTTYPE_RANGESLIDER:
            //int can be converted to float
            (*(InputSliderWidget*)m_pInput).setValue(value);
            break;
        case INPUTTYPE_DROPDOWN:
            (*(InputDropDownWidget*)m_pInput).setValue(value);
        default:
            //invalid type
            break;
        }
    }
    void LabelWidget::setDefault(bool value)
    {
        if (m_type == INPUTTYPE_BOOL) {
            (*(InputCheckboxWidget*)m_pInput).setState(value);
        }
    }
    void LabelWidget::setDefault(std::u32string value)
    {
        if (m_type == INPUTTYPE_STRING) {
            (*(InputTextWidget*)m_pInput).setText(value);
        }
    }
    void LabelWidget::setDefault(float value)
    {
        if (m_type == INPUTTYPE_RANGESLIDER) {
            (*(InputSliderWidget*)m_pInput).setValue(value);
        }
    }
    void LabelWidget::setStepSize(float stepSize)
    {
        if (m_type == INPUTTYPE_RANGESLIDER) {
            (*(InputSliderWidget*)m_pInput).setStepSize(stepSize);
        }
    }
    void LabelWidget::setOptions(std::map<int, std::u32string> optionMap)
    {
        if (m_type == INPUTTYPE_DROPDOWN) {
            (*(InputDropDownWidget*)m_pInput).setOptions(optionMap);
        }
    }

    // template<typename T> auto LabelWidget::getInputWidget()
    // {
    //     switch (m_type) {
    //         case INPUTTYPE_INT:
    //             return (InputNumberWidget*)m_pInput;
    //         case INPUTTYPE_BOOL:
    //             return (InputCheckboxWidget*)m_pInput;
    //         case INPUTTYPE_STRING:
    //             return (InputTextWidget*)m_pInput;
    //         case INPUTTYPE_RANGESLIDER:
    //             return (InputSliderWidget*)m_pInput;
    //         default:
    //             return nullptr;
    //     }
    // }


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

    void LabelWidget::childValueChanged(BaseWidget* child)
    {
        //replace with this LabelWidget
        if (m_parent != nullptr) m_parent->childValueChanged(this);
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
            m_width = m_justification + m_pInput->getWidth();
            m_height = std::max(m_pInput->getHeight(), m_height);
        }
        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void LabelWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_pLabelText->draw(renderDevice);
        m_pInput->draw(renderDevice);
    }

}//name space