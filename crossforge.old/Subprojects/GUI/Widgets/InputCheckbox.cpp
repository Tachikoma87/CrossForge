#include "InputCheckbox.h"

namespace CForge {

    InputCheckboxWidget::InputCheckboxWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        WidgetStyle defaults;
        m_state = false;
        m_checkmark = new TextWidget(m_root, this);
        m_checkmark->setText(U"x");
        m_checkmark->changePosition(defaults.WithinWidgetPadding, 0);
        m_width = m_checkmark->getWidth() + 2 * defaults.WithinWidgetPadding;
        m_height = m_checkmark->getHeight();
        m_background = new WidgetBackgroundBorder(this, m_root);
        m_root->registerMouseDownEvent(this);
    }
    InputCheckboxWidget::~InputCheckboxWidget()
    {
        delete m_checkmark;
        delete m_background;
    }
    bool InputCheckboxWidget::getValue()
    {
        return m_state;
    }
    void InputCheckboxWidget::setState(bool state)
    {
        m_state = state;

        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }

    void InputCheckboxWidget::onClick(mouseEventInfo)
    {
        m_state = !m_state;

        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }
    void InputCheckboxWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        m_checkmark->changePosition(dx, dy);
        m_background->updatePosition();
    }
    void InputCheckboxWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        if (m_state) m_checkmark->draw(renderDevice);
    }
}//name space

