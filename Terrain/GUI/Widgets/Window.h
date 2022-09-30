/*****************************************************************************\
*                                                                           *
* File(s): Window.h and Window.cpp                                      *
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
#ifndef __CFORGE_WINDOW_H__
#define __CFORGE_WINDOW_H__

#include "../GUI.h"
#include "../Widget.h"

namespace CForge {
    class WindowWidget;

    class WindowWidget_MinimizeButton : public TextWidget {
    public:
        WindowWidget_MinimizeButton(WindowWidget* window, GUI* rootGUIObject, BaseWidget* parent);
        ~WindowWidget_MinimizeButton();

        void onClick(mouseEventInfo) override;

    private:
        void changeAppearance();
        bool m_minimized;
        WindowWidget* m_window;
    };

    class WindowWidget_Header : public BaseWidget {
    public:
        WindowWidget_Header(std::u32string title, GUI* rootGUIObject, WindowWidget* parent);
        ~WindowWidget_Header();

        void updateChildPositions();

        void onDrag(mouseEventInfo mouse) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

    private:
        WindowWidget* m_window;
        TextWidget* m_title;
        WindowWidget_MinimizeButton* m_button;
    };

    class WindowWidget : public BaseWidget {
    public:
        WindowWidget(std::u32string title, GUI* rootGUIObject, BaseWidget* parent);
        ~WindowWidget();

        void setContentWidget(BaseWidget* content);
        void minimize(bool state);
        bool isMinimized();

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

    private:
        bool m_minimized;
        WindowWidget_Header* m_header;
        WidgetBackgroundBorder* m_border;
        BaseWidget* m_content;
    };

}//name space
#endif