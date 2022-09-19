#pragma once

#include "../Widget.h"
#include "../GUI.h"

class InputDropDownWidget;

class InputDropDownWidget_PopupEntry : public TextWidget {
public:
    InputDropDownWidget_PopupEntry(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown);
    void onClick(mouseEventInfo) override;
    void setOption(int id, std::u32string text, bool active = false);
private:
    int m_OptionID;
    InputDropDownWidget* m_pDropdown;
};

class InputDropDownWidget_PopupBox : public BaseWidget {
public:
    InputDropDownWidget_PopupBox(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown);
    ~InputDropDownWidget_PopupBox();
    void createEntries(std::map<int, std::u32string> optionMap);

    void changePosition(float dx, float dy) override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    std::vector<InputDropDownWidget_PopupEntry*> m_entries;
    InputDropDownWidget* m_pDropdown;
    WidgetBackgroundBorder m_border;
};

class InputDropDownWidget_Popup : public BaseWidget {
public:
    InputDropDownWidget_Popup(GUI* rootGUIObject, InputDropDownWidget* parent);
    ~InputDropDownWidget_Popup();

    void createEntries(std::map<int, std::u32string> optionMap);
    void onClick(mouseEventInfo mouse) override;

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    InputDropDownWidget* m_pDropdown;
    InputDropDownWidget_PopupBox* m_box;
};

class InputDropDownWidget : public BaseWidget {
public:
    InputDropDownWidget(GUI* rootGUIObject, BaseWidget* parent);
    ~InputDropDownWidget();

    void setValue(int value);
    int getValue();
    void setOptions(std::map<int, std::u32string> optionMap);
//     void focus() override;
//     void focusLost() override;
    void onClick(mouseEventInfo) override;
    void closePopup();
    void closePopup(int newValue);

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    int m_value;
    std::map<int, std::u32string> m_options;
    TextWidget* m_pDisplay;
};
