#pragma once

#include "Widget.h"
#include "WidgetBackground.h"
#include "Font.h"
#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Input/Mouse.h>
#include <vector>
class BaseWidget;

class GUI {
public:
    enum Event {
        CLICK
    };
    GUI(CForge::RenderDevice* renderDevice);
    ~GUI();
    void testInit();
    void testRender();
    void processMouseEvents(CForge::Mouse* mouse);
    void registerMouseDownEvent(BaseWidget* widget);
    
    CForge::GLShader* BackgroundColoredShader;
    CForge::GLShader* TextShader;
    FontFace* fontFace;
private:
    std::vector<BaseWidget*> testBG;
    CForge::RenderDevice* m_renderDevice;
    std::vector<BaseWidget*> m_events_mouseDown;

    TextLine testtext;
};
