#pragma once

#include "../Widget.h"
#include "../GUI.h"
#include "../Callback.h"

#include "Label.h"
#include <CForge/Core/ITCaller.hpp>

class FormWidget;

class FormWidget_ApplyButton : public TextWidget {
public:
    FormWidget_ApplyButton(GUI* rootGUIObject, FormWidget* parent);
    void onClick(mouseEventInfo) override;
private:
    FormWidget* m_parentForm;
};

class FormWidget : public BaseWidget, public CForge::ITCaller<CallbackObject> {
public:
    FormWidget(int FormID, GUI* rootGUIObject, BaseWidget* parent);
    ~FormWidget();

    void addOption(int OptionID, CallbackDatatype type, std::u32string name);
    void setLimit(int OptionID, int higher);
    void setLimit(int OptionID, int lower, int higher);
    void setDefault(int OptionID, int value);
    void setDefault(int OptionID, bool value);
    void setDefault(int OptionID, std::u32string value);
    void sendCallback();

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;

    //for testing
    void onDrag(mouseEventInfo mouse) override;
private:
    int m_FormID;
    std::unordered_map<int, LabelWidget*> m_labels;
    FormWidget_ApplyButton* m_ApplyButton;
};

