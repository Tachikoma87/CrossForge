#include "InputText.h"

namespace CForge {

    InputTextWidget_ClearButton::InputTextWidget_ClearButton(GUI* rootGUIObject, InputTextWidget* parent) : TextWidget(rootGUIObject, parent)
    {
        m_text = parent;
        rootGUIObject->registerMouseDownEvent(this);
        setText(U"x");
        m_background = new WidgetBackgroundBorder(this, m_root);
    }
    InputTextWidget_ClearButton::~InputTextWidget_ClearButton()
    {
        delete m_background;
    }
    void InputTextWidget_ClearButton::onClick(mouseEventInfo)
    {
        m_text->clearText();
    }




    InputTextWidget::InputTextWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_text = new TextWidget(m_root, this);
        m_clear = new InputTextWidget_ClearButton(m_root, this);
        m_height = m_clear->getHeight();
        m_background = new WidgetBackgroundBorder(this, m_root);
        recalculateLayout();
        m_limits.min = 0;
        m_limits.max = 255;     //not like that's an actual limit, can be overwritten if longer text is required.
        m_root->registerKeyPressEvent(this);
    }
    InputTextWidget::~InputTextWidget()
    {
        delete m_text;
        delete m_clear;
        delete m_background;
    }
    std::u32string InputTextWidget::getValue()
    {
        return m_value;
    }
    bool InputTextWidget::validateInput()
    {
        //useful for derived inputs like a float input or whatever

        //for the plain text input, limits on the text length can be imposed
        InputTextStyle defaults;
        std::u32string newValue = m_text->getText();
        bool valid = newValue.length() <= m_limits.max && newValue.length() >= m_limits.min;
        if (valid) m_value = newValue;
        if (m_parent != nullptr) m_parent->childValueChanged(this);
        return valid;
    }
    void InputTextWidget::clearText()
    {
        setText(U"");
    }
    void InputTextWidget::setText(std::u32string text)
    {
        m_value = text;
        m_text->setText(text);
        updateLayout();

        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }

    void InputTextWidget::focus()
    {
        FontStyle1 defaults;
        m_text->setColor(defaults.FontColorHighlight);
    }
    void InputTextWidget::focusLost()
    {
        if (validateInput()) {
            FontStyle1 defaults;
            m_text->setColor(defaults.FontColor);
        }
        else {
            InputTextStyle defaults;
            m_text->setColor(defaults.WrongInputColor);
        }
    }
    void InputTextWidget::onKeyPress(char32_t c)
    {
        if (c == U'\n') {
            InputTextStyle defaults;
            if (validateInput()) {
                m_text->setColor(defaults.ValidInputColor);
            }
            else {
                m_text->setColor(defaults.WrongInputColor);
            }
        }
        else {
            m_text->changeText(c);
        }
        updateLayout();
    }
    void InputTextWidget::setLimit(int lower, int higher)
    {
        m_limits.min = lower;
        m_limits.max = higher;
    }
    void InputTextWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        m_text->changePosition(dx, dy);
        m_clear->changePosition(dx, dy);
        m_background->updatePosition();
    }
    void InputTextWidget::recalculateLayout()
    {
        m_width = std::max(m_text->getWidth(), 30.0f);
        float clearMovement = m_x + m_width - m_clear->getPosition()[0];
        m_width += m_clear->getWidth();
        m_clear->changePosition(clearMovement, 0);
        m_background->updateSize();
    }
    void InputTextWidget::updateLayout()
    {
        recalculateLayout();
        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void InputTextWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_text->draw(renderDevice);
        m_clear->draw(renderDevice);
    }

}//name space