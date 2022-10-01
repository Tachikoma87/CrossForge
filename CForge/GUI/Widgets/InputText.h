/*****************************************************************************\
*                                                                           *
* File(s): InputText.h and InputText.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTTEXT_H__
#define __CFORGE_INPUTTEXT_H__

#include "../Widget.h"
#include "../GUI.h"

namespace CForge {
    class InputTextWidget;

    class CFORGE_API InputTextWidget_ClearButton : public TextWidget {
    public:
        InputTextWidget_ClearButton(GUI* rootGUIObject, InputTextWidget* parent);
        ~InputTextWidget_ClearButton();

        void onClick(mouseEventInfo) override;

    private:
        InputTextWidget* m_text;
    };

    class CFORGE_API InputTextWidget : public BaseWidget {
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

}//name space
#endif