#include "Form.h"
#include "../Callback.h"

FormWidget_ApplyButton::FormWidget_ApplyButton(GUI* rootGUIObject, FormWidget* parent) : TextWidget(rootGUIObject, parent)
{
    setText(U"Apply");
    rootGUIObject->registerMouseDownEvent(this);
    m_parentForm = parent;
}
void FormWidget_ApplyButton::onClick(mouseEventInfo)
{
    m_parentForm->sendCallback();
}



FormWidget::FormWidget(int FormID, GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
{
    m_FormID = FormID;
    m_ApplyButton = new FormWidget_ApplyButton(rootGUIObject, this);
    m_ApplyButton->setText(U"Apply");
    m_height = m_ApplyButton->getHeight();
    m_width = m_ApplyButton->getWidth();
}
FormWidget::~FormWidget()
{
    for (auto x : m_labels) {
        delete x.second;
    }
    delete m_ApplyButton;
}

void FormWidget::addOption(int OptionID, CallbackDatatype type, std::u32string name)
{
    //do not create options with colliding IDs, they won't work either way
    if (m_labels.count(OptionID) > 0) return;

    LabelWidget* label = new LabelWidget(type, name, m_root, this);
    float labelOffset = m_height - m_ApplyButton->getHeight();
    label->changePosition(0, labelOffset);
    m_labels.emplace(OptionID, label);
    m_ApplyButton->changePosition(0, label->getHeight());
    m_height += label->getHeight();
    m_width = std::max(m_width, label->getWidth());
}
void FormWidget::sendCallback()
{
    CallbackObject bcObj;
    bcObj.FormID = m_FormID;
    bcObj.Data.clear();
    for (auto x : m_labels) {
        bcObj.Data.emplace(x.first, x.second->getValue());
    }
    broadcast(bcObj);
}

void FormWidget::changePosition(float dx, float dy)
{
    m_x += dx;
    m_y += dy;
    for (auto x : m_labels) {
        x.second->changePosition(dx, dy);
    }
    m_ApplyButton->changePosition(dx, dy);
}
void FormWidget::draw(CForge::RenderDevice* renderDevice)
{
    for (auto x : m_labels) {
        x.second->draw(renderDevice);
    }
    m_ApplyButton->draw(renderDevice);
}



void FormWidget::onDrag(mouseEventInfo mouse)
{
    setPosition(mouse.adjustedPosition[0], mouse.adjustedPosition[1]);
}


