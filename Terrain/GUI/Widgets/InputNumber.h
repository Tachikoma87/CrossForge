#pragma once

#include "../Widget.h"
#include "../GUI.h"

#include <CForge/Input/Mouse.h>

class InputNumberWidget;

class InputNumberWidget_DecreaseButton : public TextWidget {
public:
    InputNumberWidget_DecreaseButton(GUI* rootGUIObject, InputNumberWidget* parent);
    void onClick(CForge::Mouse * ) override;
private:
    InputNumberWidget* m_parentInput;
};

class InputNumberWidget_IncreaseButton : public TextWidget {
public:
    InputNumberWidget_IncreaseButton(GUI* rootGUIObject, InputNumberWidget* parent);
    void onClick(CForge::Mouse * ) override;
private:
    InputNumberWidget* m_parentInput;
};

class InputNumberWidget : public BaseWidget {
public:
    InputNumberWidget(GUI* rootGUIObject, BaseWidget* parent);
    void setValue(int value);
    void changeValue(int value);
    int getValue();
    void setPosition(float x, float y);
    void changePosition(float dx, float dy);
    void draw(CForge::RenderDevice* renderDevice);
private:
    int m_value;
    InputNumberWidget_DecreaseButton* m_pDec;
    InputNumberWidget_IncreaseButton* m_pInc;
    TextWidget* m_pValue;
};

class TestWidget : public BaseWidget {
public:
    TestWidget(GUI* rootGUIObject, BaseWidget* parent);
    void onDrag(CForge::Mouse* mouse);
};
