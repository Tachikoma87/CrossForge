#include "Form.h"

namespace CForge {

    FormWidget_ApplyButton::FormWidget_ApplyButton(GUI* rootGUIObject, FormWidget* parent) : TextWidget(rootGUIObject, parent)
    {
        changeFont(GUI::FONT1_BOLD);
        rootGUIObject->registerMouseDownEvent(this);
        m_parentForm = parent;
    }
    void FormWidget_ApplyButton::onClick(mouseEventInfo)
    {
        m_parentForm->sendCallback();
    }



    FormWidget::FormWidget(int FormID, GUI* rootGUIObject, BaseWidget* parent, std::u32string applyName) : BaseWidget(rootGUIObject, parent)
    {
        m_FormID = FormID;
        m_ApplyButton = nullptr;
        if (applyName.empty()) {
            m_width = 0;
            m_height = 0;
        }
        else {
            m_ApplyButton = new FormWidget_ApplyButton(rootGUIObject, this);
            m_ApplyButton->setText(applyName);
            m_height = m_ApplyButton->getHeight();
            m_width = m_ApplyButton->getWidth();
        }
        //     m_background = new WidgetBackgroundColored(this, m_root->BackgroundColoredShader);
    }
    FormWidget::~FormWidget()
    {
        for (auto x : m_labels) {
            delete x.second;
        }
        delete m_ApplyButton;
        //     delete m_background;
    }

    void FormWidget::addOption(int OptionID, GUIInputType type, std::u32string name)
    {
        //prevent sending callbacks during init
        dontSendCallback = true;

        //do not create options with colliding IDs, they won't work either way
        if (m_labels.count(OptionID) > 0) return;

        LabelWidget* label = new LabelWidget(type, name, m_root, this);
        float labelOffset = m_height - (m_ApplyButton ? m_ApplyButton->getHeight() : 0);
        label->changePosition(0, labelOffset);
        m_labels.emplace(OptionID, label);
        if (m_ApplyButton) m_ApplyButton->changePosition(0, label->getHeight());
        float justification = label->getJustification();
        for (auto x : m_labels) {
            justification = std::max(x.second->getJustification(), justification);
        }
        for (auto x : m_labels) {
            x.second->setJustification(justification);
        }
        updateLayout();
        //     m_background->updateSize();
    }
    // template<typename T>
    // auto FormWidget::addOption(int OptionID, GUIInputType type, std::u32string name)
    // {
    //     addOption(OptionID, type, name);
    //     return m_labels.at(OptionID)->getInputWidget<T>();
    // }

    void FormWidget::setLimit(int OptionID, int higher)
    {
        dontSendCallback = true;
        if (higher > 0) {
            setLimit(OptionID, 0, higher);
        }
        else {
            setLimit(OptionID, higher, 0);
        }
        dontSendCallback = false;
    }
    void FormWidget::setLimit(int OptionID, int lower, int higher)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setLimit(lower, higher);
        }
        dontSendCallback = false;
    }
    void FormWidget::setLimit(int OptionID, float higher)
    {
        dontSendCallback = true;
        if (higher > 0) {
            setLimit(OptionID, 0.0f, higher);
        }
        else {
            setLimit(OptionID, higher, 0.0f);
        }
        dontSendCallback = false;
    }
    void FormWidget::setLimit(int OptionID, float lower, float higher)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setLimit(lower, higher);
        }
        dontSendCallback = false;
    }
    void FormWidget::setDefault(int OptionID, int value)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setDefault(value);
        }
        dontSendCallback = false;
    }
    void FormWidget::setDefault(int OptionID, float value)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setDefault(value);
        }
        dontSendCallback = false;
    }
    void FormWidget::setDefault(int OptionID, bool value)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setDefault(value);
        }
        dontSendCallback = false;
    }
    void FormWidget::setDefault(int OptionID, std::u32string value)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setDefault(value);
        }
        dontSendCallback = false;
    }
    void FormWidget::setStepSize(int OptionID, float stepSize)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setStepSize(stepSize);
        }
        dontSendCallback = false;
    }
    void FormWidget::setDropDownOptions(int OptionID, std::map<int, std::u32string> optionMap)
    {
        dontSendCallback = true;
        if (m_labels.count(OptionID) > 0) {
            m_labels[OptionID]->setOptions(optionMap);
        }
        dontSendCallback = false;
    }


    void FormWidget::sendCallback()
    {
        if (dontSendCallback) return;

        GUICallbackObject bcObj;
        bcObj.FormID = m_FormID;
        bcObj.Data.clear();
        for (auto x : m_labels) {
            bcObj.Data.emplace(x.first, x.second->getValue());
        }
        broadcast(bcObj);
    }

    void FormWidget::childValueChanged(BaseWidget* child)
    {
        if (!m_ApplyButton && !dontSendCallback) {
            sendCallback();
        }
    }

    void FormWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        for (auto x : m_labels) {
            x.second->changePosition(dx, dy);
        }
        if (m_ApplyButton) m_ApplyButton->changePosition(dx, dy);
        //     m_background->setPosition(m_x, m_y);
    }
    void FormWidget::updateLayout()
    {
        m_width = m_ApplyButton ? m_ApplyButton->getWidth() : 0;
        m_height = m_ApplyButton ? m_ApplyButton->getHeight() : 0;
        for (auto x : m_labels) {
            m_width = std::max(m_width, x.second->getWidth());
            m_height += x.second->getHeight();
        }
        //     m_background->updateSize();
        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void FormWidget::draw(CForge::RenderDevice* renderDevice)
    {
        //     m_background->render(renderDevice);
        for (auto x : m_labels) {
            x.second->draw(renderDevice);
        }
        if (m_ApplyButton) m_ApplyButton->draw(renderDevice);
    }



    void FormWidget::onDrag(mouseEventInfo mouse)
    {
        setPosition(mouse.adjustedPosition[0], mouse.adjustedPosition[1]);
    }

}//name space
