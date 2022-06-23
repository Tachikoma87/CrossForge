#pragma once

#include "../Widget.h"
#include "../GUI.h"
#include "InputText.h"

class InputSliderWidget;

class InputSliderWidget_SliderCursor : public BaseWidget {
public:
    InputSliderWidget_SliderCursor(GUI* rootGUIObject, BaseWidget* parent);
    ~InputSliderWidget_SliderCursor();
};

class InputSliderWidget_Slider : public BaseWidget {
public:
    InputSliderWidget_Slider(GUI* rootGUIObject, InputSliderWidget* parent);
    ~InputSliderWidget_Slider();

    void setCursorPosition(float percantage);
    void onClick(mouseEventInfo mouse) override;
    void onDrag(mouseEventInfo mouse) override;

    void changePosition(float dx, float dy) override;
//     void updateLayout() override;        //we won't change the slider's dimensions
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    InputSliderWidget* m_slider;
    InputSliderWidget_SliderCursor* m_cursor;
};

class InputSliderWidget_Text : public InputTextWidget {
public:
    InputSliderWidget_Text(GUI* rootGUIObject, InputSliderWidget* parent);
    ~InputSliderWidget_Text();

    float getFloatValue();
    void setFloatValue(float value);

private:
    bool validateInput() override;
    float m_floatValue;
    struct {
        float min;
        float max;
    } m_limits;
    InputSliderWidget* m_slider;
};

class InputSliderWidget : public BaseWidget {
public:
    InputSliderWidget(GUI* rootGUIObject, BaseWidget* parent);
    ~InputSliderWidget();

    float getValue();
    void setValue(float value);
    void setValueByPercentage(float sliderPercantage);
    void setLimit(int lower, int higher);
    void setLimit(float lower, float higher);
    void setStepSize(float stepSize);

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    float m_value;
    struct {
        float min;
        float max;
    } m_limits;
    float m_stepSize;

    InputSliderWidget_Text* m_text;
    InputSliderWidget_Slider* m_slide;
};

