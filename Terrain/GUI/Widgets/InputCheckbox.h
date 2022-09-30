/*****************************************************************************\
*                                                                           *
* File(s): InputCheckbox.h and InputCheckbox.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): SimonKretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTCHECKBOX_H__
#define __CFORGE_INPUTCHECKBOX_H__

#include "../Widget.h"
#include "../GUI.h"

namespace CForge {

    class InputCheckboxWidget : public BaseWidget {
    public:
        InputCheckboxWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputCheckboxWidget();

        bool getValue();
        void setState(bool state);

        void onClick(mouseEventInfo) override;

        void changePosition(float dx, float dy) override;
        /*void updateLayout() override;*/   //this widget does not change its size
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        bool m_state;
        TextWidget* m_checkmark;
    };

}//name space
#endif