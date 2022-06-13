#pragma once

#include "../Widget.h"
#include "../GUI.h"
#include "../Callback.h"

#include "InputNumber.h"
#include "InputCheckbox.h"
#include "InputText.h"

class LabelWidget : public BaseWidget {
public:
    LabelWidget(CallbackDatatype type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent);
    ~LabelWidget();

    CallbackDatum getValue();
    void setLimit(int lower, int higher);
    void setDefault(int value);
    void setDefault(bool value);

    float getJustification();
    void setJustification(float j);

    void changePosition(float dx, float dy) override;
    void updateLayout() override;
    void draw(CForge::RenderDevice* renderDevice) override;
private:
    CallbackDatatype m_type;
    void * m_pValue;
    TextWidget* m_pLabelText;
    BaseWidget* m_pInput;
    float m_justification;      //offset of the input element
};
