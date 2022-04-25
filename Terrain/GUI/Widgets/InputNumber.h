#pragma once

#include "../Widget.h"
#include "../GUI.h"

#include <CForge/Input/Mouse.h>

class InputNumber;

class InputNumber_DecreaseButton : public TextWidget {
public:
    InputNumber_DecreaseButton(GUI* rootGUIObject, InputNumber* parent);
    void onClick(CForge::Mouse * ) override;
private:
    InputNumber* m_parentInput;
};

class InputNumber_IncreaseButton : public TextWidget {
public:
    InputNumber_IncreaseButton(GUI* rootGUIObject, InputNumber* parent);
    void onClick(CForge::Mouse * ) override;
private:
    InputNumber* m_parentInput;
};

class InputNumber : public BaseWidget {
public:
    InputNumber(GUI* rootGUIObject, BaseWidget* parent);
    void setValue(int value);
    void changeValue(int value);
    int getValue();
    void setPosition(float x, float y);
    void changePosition(float dx, float dy);
    void draw(CForge::RenderDevice* renderDevice);
private:
    int m_value;
    InputNumber_DecreaseButton* m_pDec;
    InputNumber_IncreaseButton* m_pInc;
    TextWidget* m_pValue;
};
