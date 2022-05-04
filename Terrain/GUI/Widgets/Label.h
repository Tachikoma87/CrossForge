#pragma once

#include "../Widget.h"
#include "../GUI.h"
#include "../Callback.h"

#include "InputNumber.h"

class LabelWidget : public BaseWidget {
public:
    LabelWidget(CallbackDatatype type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent);
    ~LabelWidget();

    CallbackDatum getValue();

    void changePosition(float dx, float dy);
    void draw(CForge::RenderDevice* renderDevice);
private:
    CallbackDatatype m_type;
    void * m_pValue;
    TextWidget* m_pLabelText;
    BaseWidget* m_pInput;
};
