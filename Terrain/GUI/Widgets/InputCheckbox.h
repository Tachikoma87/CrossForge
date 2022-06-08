#pragma once

#include "../Widget.h"
#include "../GUI.h"

class InputCheckboxWidget : public BaseWidget {
public:
    InputCheckboxWidget(GUI* rootGUIObject, BaseWidget* parent);
    ~InputCheckboxWidget();

    bool getValue();

    void onClick(mouseEventInfo) override;

    void changePosition(float dx, float dy) override;
    /*void updateLayout() override;*/   //this widget does not change its size
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    bool m_state;
    TextWidget* m_checkmark;
};

