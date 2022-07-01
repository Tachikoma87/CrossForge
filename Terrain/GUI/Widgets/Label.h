#pragma once

#include "../Widget.h"
#include "../GUI.h"
#include "../Callback.h"

#include "InputNumber.h"
#include "InputCheckbox.h"
#include "InputText.h"
#include "InputSlider.h"
#include "InputDropDown.h"

class LabelWidget : public BaseWidget {
public:
    LabelWidget(GUIInputType type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent);
    ~LabelWidget();

    GUICallbackDatum getValue();
    void setLimit(int lower, int higher);
    void setLimit(float lower, float higher);
    void setDefault(int value);
    void setDefault(float value);
    void setDefault(bool value);
    void setDefault(std::u32string value);
    void setStepSize(float stepSize);
    void setOptions(std::map<int, std::u32string> optionMap);

//     template <typename T> auto getInputWidget();

    float getJustification();
    void setJustification(float j);

    void childValueChanged(BaseWidget* child) override;

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    GUIInputType m_type;
    void * m_pValue;
    TextWidget* m_pLabelText;
    BaseWidget* m_pInput;
    float m_justification;      //offset of the input element
};
