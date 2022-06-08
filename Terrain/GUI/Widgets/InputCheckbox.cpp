#include "InputCheckbox.h"

InputCheckboxWidget::InputCheckboxWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
{
    WidgetStyle defaults;
    m_state = false;
    m_checkmark = new TextWidget(m_root, this);
    m_checkmark->setText(U"x");
    m_checkmark->changePosition(defaults.WithinWidgetPadding, 0);
    m_width = m_checkmark->getWidth() + 2*defaults.WithinWidgetPadding;
    m_height = m_checkmark->getHeight();
    m_background = new WidgetBackgroundBorder(this, m_root->BackgroundColoredShader);
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
void InputCheckboxWidget::onClick(mouseEventInfo)
{
    m_state = !m_state;
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
    m_background->render(renderDevice);
    if (m_state) m_checkmark->draw(renderDevice);
}

