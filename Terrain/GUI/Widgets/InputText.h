#pragma once

#include "../Widget.h"
#include "../GUI.h"
class InputTextWidget;

class InputTextWidget_ClearButton : public TextWidget {
public:
    InputTextWidget_ClearButton(GUI* rootGUIObject, InputTextWidget* parent);
    ~InputTextWidget_ClearButton();

    void onClick(mouseEventInfo) override;

private:
    InputTextWidget* m_text;
};

class InputTextWidget : public BaseWidget {
public:
    InputTextWidget(GUI* rootGUIObject, BaseWidget* parent);
    ~InputTextWidget();

    std::u32string getValue();
    void setLimit(int lower, int higher);
    void clearText();
    void setText(std::u32string text);

//     void onClick(mouseEventInfo mouse) override;
    void onKeyPress(char32_t c) override;
    void focus() override;
    void focusLost() override;

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
protected:
    virtual bool validateInput();
    std::u32string m_value;
    TextWidget* m_text;
private:
    void recalculateLayout();

    struct {
        int min;
        int max;
    } m_limits;
    InputTextWidget_ClearButton* m_clear;
};
