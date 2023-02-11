#include "InputDropDown.h"

namespace CForge {

    InputDropDownWidget_PopupEntry::InputDropDownWidget_PopupEntry(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown)
        : TextWidget(rootGUIObject, parent)
    {
        m_pDropdown = dropdown;
        m_root->registerMouseDownEvent(this);
    }
    void InputDropDownWidget_PopupEntry::setOption(int id, std::u32string text, bool active)
    {
        m_OptionID = id;
        if (active) {
            changeFont(GUI::FONT1_BOLD);
        }
        else {
            changeFont(GUI::FONT1_REGULAR);
        }
        setText(text);
    }
    void InputDropDownWidget_PopupEntry::onClick(mouseEventInfo)
    {
        m_pDropdown->closePopup(m_OptionID);
    }


    InputDropDownWidget_PopupBox::InputDropDownWidget_PopupBox(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown)
        : BaseWidget(rootGUIObject, parent),
        m_border(this, m_root)
    {
        m_pDropdown = dropdown;
        m_background = new WidgetBackgroundColored(this, m_root);
    }
    InputDropDownWidget_PopupBox::~InputDropDownWidget_PopupBox()
    {
        for (auto x : m_entries) {
            delete x;
        }
        delete m_background;
    }

    void InputDropDownWidget_PopupBox::createEntries(std::map<int, std::u32string> optionMap)
    {
        WidgetStyle defaults;
        if (optionMap.size() > 0) {
            //there shouldn't be any leftovers in there, but clear it to be safe
            for (auto x : m_entries) delete x;
            m_entries.clear();
            int activeOption = m_pDropdown->getValue();
            auto it = optionMap.begin();
            Eigen::Vector2f offset = getPosition();
            //add some padding
            offset.x() += defaults.WithinWidgetPadding;
            offset.y() += defaults.WithinWidgetPadding;
            m_height = defaults.WithinWidgetPadding;
            m_width = 0;
            while (it != optionMap.end()) {
                bool active = it->first == activeOption;
                InputDropDownWidget_PopupEntry* x = new InputDropDownWidget_PopupEntry(m_root, this, m_pDropdown);
                x->setOption(it->first, it->second, active);
                x->setPosition(offset[0], offset[1]);
                offset.y() += x->getHeight() + defaults.WithinWidgetPadding;
                m_height += x->getHeight() + defaults.WithinWidgetPadding;
                m_width = std::max(m_width, x->getWidth());
                m_entries.push_back(x);
                it++;
            }
            m_width += 2 * defaults.WithinWidgetPadding;
        }
        else {
            m_entries.clear();
            m_height = 2 * defaults.WithinWidgetPadding;
            m_width = 2 * defaults.WithinWidgetPadding;
        }
        m_background->updateSize();
        m_border.updateSize();
    }
    void InputDropDownWidget_PopupBox::changePosition(float dx, float dy)
    {
        BaseWidget::changePosition(dx, dy);
        m_border.updatePosition();
        for (auto entry : m_entries) {
            entry->changePosition(dx, dy);
        }
    }
    void InputDropDownWidget_PopupBox::draw(CForge::RenderDevice* renderDevice)
    {
        if (m_background != nullptr) m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_border.render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        for (auto entry : m_entries) {
            entry->draw(renderDevice);
        }
    }


    InputDropDownWidget_Popup::InputDropDownWidget_Popup(GUI* rootGUIObject, InputDropDownWidget* parent)
        : BaseWidget(rootGUIObject, parent)
    {
        m_pDropdown = parent;
        m_parent = nullptr; //this should make this widget register as top level widget in the gui class
        m_box = new InputDropDownWidget_PopupBox(rootGUIObject, this, parent);
        //take the whole available screen for input
        m_x = 0;
        m_y = 0;
        m_width = m_root->getWindowWidth();
        m_height = m_root->getWindowHeight();
        m_root->registerMouseDownEvent(this);
    }
    InputDropDownWidget_Popup::~InputDropDownWidget_Popup()
    {
        delete m_box;
    }
    void InputDropDownWidget_Popup::createEntries(std::map<int, std::u32string> optionMap)
    {
        m_box->createEntries(optionMap);
    }
    void InputDropDownWidget_Popup::onClick(mouseEventInfo mouse)
    {
        m_pDropdown->closePopup();
    }
    void InputDropDownWidget_Popup::changePosition(float dx, float dy)
    {
        //don't do anything, this popup should not be movable
    }
    void InputDropDownWidget_Popup::updateLayout()
    {
        //likewise, there's nothing to update sizes for
    }
    void InputDropDownWidget_Popup::draw(CForge::RenderDevice* renderDevice)
    {
        m_box->draw(renderDevice);
    }


    InputDropDownWidget::InputDropDownWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_pDisplay = new TextWidget(m_root, this);
        m_pDisplay->setText(U"... ▾");

        m_width = m_pDisplay->getWidth();
        m_height = m_pDisplay->getHeight();
        m_value = 0;

        WidgetBackgroundBorder* border = new WidgetBackgroundBorder(this, m_root);
        m_background = border;

        m_root->registerMouseDownEvent(this);
    }
    InputDropDownWidget::~InputDropDownWidget()
    {
        delete m_background;
        delete m_pDisplay;
    }
    int InputDropDownWidget::getValue()
    {
        return m_value;
    }
    void InputDropDownWidget::setValue(int value)
    {
        if (m_options.count(value) > 0) {
            m_value = value;
            m_pDisplay->setText(m_options.at(value) + U" ▾");
            updateLayout();
        }
        if (m_parent != nullptr) m_parent->childValueChanged(this);
    }
    void InputDropDownWidget::setOptions(std::map<int, std::u32string> optionMap)
    {
        m_options = optionMap;
        if (m_options.size() > 0) {
            setValue(m_options.begin()->first);
        }
        else {
            m_value = 0;
            m_pDisplay->setText(U"... ▾");
            updateLayout();
        }
    }
    void InputDropDownWidget::onClick(mouseEventInfo)
    {
        //open the popup

        //the pointer is not saved in this class. it's registered in and will be freed by the GUI class.
        //not saving it here minimizes the risk of accidentally accessing it after it's been freed.
        InputDropDownWidget_Popup* popup = new InputDropDownWidget_Popup(m_root, this);
        m_root->registerWidgetAsPopup(popup);
        popup->createEntries(m_options);

    }
    void InputDropDownWidget::closePopup()
    {
        //this currently also frees the respective widget and associated events
        m_root->unregisterPopup();
    }
    void InputDropDownWidget::closePopup(int newValue)
    {
        setValue(newValue);
        closePopup();
    }
    void InputDropDownWidget::changePosition(float dx, float dy)
    {
        BaseWidget::changePosition(dx, dy);
        m_pDisplay->changePosition(dx, dy);
    }
    void InputDropDownWidget::updateLayout()
    {
        m_width = m_pDisplay->getWidth();
        m_height = m_pDisplay->getHeight();
        m_background->updateSize();
        if (m_parent != nullptr) m_parent->updateLayout();
    }
    void InputDropDownWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_pDisplay->draw(renderDevice);
    }

}//name space