/*****************************************************************************\
*                                                                           *
* File(s): GUI.h and GUI.cpp                                      *
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
#ifndef __CFORGE_GUI_H__
#define __CFORGE_GUI_H__

namespace CForge {
    class BaseWidget;
    class TextWidget;
    class FormWidget;
}
//#include "Widget.h"

#include "WidgetBackground.h"
#include "Font.h"
#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Input/Mouse.h>
#include <CForge/Input/Character.h>
#include <vector>

#include <CForge/Core/ITListener.hpp>

/*
To be able to define callbacks of different data types without
having to define a million different individual callbacks, a
more abstract structure is needed.

For this, the GUICallbackDatum struct is introduced. The callback
code can then cast the void pointer pData back to its original
type which can be inferred from the Type attribute in form of the enum
GUIInputType (and also, the callback code should already
know which data types it expects).

The GUICallbackObject is what will actually be passed to the
callback function. It includes the FormID of the set of options
it is registered to in order to allow for the same object to
potentially receive multiple callbacks. FormIDs are set during
initialization/creation of the GUI. Likewise, the integer indices
of the Data map are the OptionIDs registered during set up of
the respective form.

The callbacks themselves are handled through CForge's ITListener
and ITCaller interface templates, with the GUICallbackObject data
type.
 */

namespace CForge {
    enum GUIInputType {
        INPUTTYPE_INT,          //returns integers
        INPUTTYPE_BOOL,         //bool
        INPUTTYPE_STRING,       //u32string
        INPUTTYPE_RANGESLIDER,  //float
        INPUTTYPE_DROPDOWN      //int
    };

    struct GUICallbackDatum {
        GUIInputType Type;
        void* pData;
    };

    struct GUICallbackObject {
        int FormID;
        std::unordered_map<int, GUICallbackDatum> Data;
    };


    struct mouseEventInfo {
        Eigen::Vector2f adjustedPosition;   //cursor position adjusted to the position within the clicked widget
        Eigen::Vector2f rawPosition;        //cursor position in window space
    };

    class CallbackTestClass : public CForge::ITListener<GUICallbackObject> {
        void listen(const GUICallbackObject Msg) override;
    };

    class GUI : public CForge::ITListener<char32_t>, public CForge::ITListener<CForge::KeyboardCallback> {
    public:
        GUI();
        ~GUI();

        void init(CForge::GLWindow* pWin);
        void render(CForge::RenderDevice* renderDevice);

        FormWidget* createOptionsWindow(std::u32string title, int FormID, std::u32string applyName = U"Apply");
        TextWidget* createPlainText();
        TextWidget* createTextWindow(std::u32string title);
        void registerWidgetAsPopup(BaseWidget* widget);
        void unregisterPopup();

        void processEvents();
        void registerMouseDownEvent(BaseWidget* widget);
        void registerMouseDragEvent(BaseWidget* widget);
        void registerKeyPressEvent(BaseWidget* widget);

        void listen(char32_t codepoint) override;
        void listen(CForge::KeyboardCallback kc) override;

        //stuff accessed by the widgets
        uint32_t getWindowWidth();
        uint32_t getWindowHeight();
        CForge::GLShader* BackgroundColoredShader;
        CForge::GLShader* TextShader;
        enum FontStyles {       //TODO: remember to keep updated when fonts are added/changed
            DEFAULT_FONT = 0,
            FONT1_REGULAR = 0,
            FONT1_BOLD = 1
        };
        std::vector<FontFace*> fontFaces;
    private:
        enum EventType {
            EVENT_CLICK,
            EVENT_DRAG,
            EVENT_KEYPRESS
        };
        void registerEvent(BaseWidget* widget, EventType et);
        //     void unregisterEvent(BaseWidget* widget, EventType et);
        void submitTopLevelWidget(BaseWidget* widget);
        void processMouseEvents(CForge::Mouse* mouse);
        void processKeyboardEvents(CForge::Keyboard* keyboard);
        void loadFonts();
        FT_Library library;

        CallbackTestClass callbackTest;

        CForge::GLWindow* m_pWin;
        BaseWidget* focusedWidget = nullptr;
        Eigen::Vector2f focusedClickOffset;

        struct TopLevelWidgetHandler {
            BaseWidget* pWidget;
            std::vector<BaseWidget*> eventsMouseDown;
            std::vector<BaseWidget*> eventsMouseDrag;
            std::vector<BaseWidget*> eventsKeyPress;
            TopLevelWidgetHandler(BaseWidget* w) {
                pWidget = w;
            }
        };
        std::vector<TopLevelWidgetHandler*> m_TopLevelWidgets;
        TopLevelWidgetHandler* m_Popup;
    };

}//name space
#endif