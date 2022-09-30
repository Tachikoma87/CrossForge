/*****************************************************************************\
*                                                                           *
* File(s): Form.h and Form.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Simon Kretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_FORM_H__
#define __CFORGE_FORM_H__

#include "../Widget.h"
#include "../GUI.h"

#include "Label.h"
#include <CForge/Core/ITCaller.hpp>

namespace CForge {

    class FormWidget;

    class FormWidget_ApplyButton : public TextWidget {
    public:
        FormWidget_ApplyButton(GUI* rootGUIObject, FormWidget* parent);
        void onClick(mouseEventInfo) override;
    private:
        FormWidget* m_parentForm;
    };

    class FormWidget : public BaseWidget, public CForge::ITCaller<GUICallbackObject> {
    public:
        FormWidget(int FormID, GUI* rootGUIObject, BaseWidget* parent, std::u32string applyName = U"Apply");
        ~FormWidget();

        void addOption(int OptionID, GUIInputType type, std::u32string name);
        //     template <typename T>
        //     auto addOption(int OptionID, GUIInputType type, std::u32string name);
        void setLimit(int OptionID, int higher);
        void setLimit(int OptionID, int lower, int higher);
        void setLimit(int OptionID, float higher);
        void setLimit(int OptionID, float lower, float higher);
        void setDefault(int OptionID, int value);
        void setDefault(int OptionID, float value);
        void setDefault(int OptionID, bool value);
        void setDefault(int OptionID, std::u32string value);
        void setStepSize(int OptionID, float stepSize);
        void setDropDownOptions(int OptionID, std::map<int, std::u32string> optionMap);
        void sendCallback();

        void childValueChanged(BaseWidget* child) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

        //for testing
        void onDrag(mouseEventInfo mouse) override;
    private:
        int m_FormID;
        std::unordered_map<int, LabelWidget*> m_labels;
        FormWidget_ApplyButton* m_ApplyButton;
        bool dontSendCallback = false;      //used for immediate callback mode to prevent calling it unnecessarily often
    };

}//name space
#endif