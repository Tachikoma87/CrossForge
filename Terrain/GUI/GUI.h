#pragma once

//#include "Widget.h"
#include "WidgetBackground.h"
#include "Font.h"
#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Input/Mouse.h>
#include <vector>
class BaseWidget;

typedef struct {
    Eigen::Vector2f adjustedPosition;   //cursor position adjusted to the position within the clicked widget
} mouseEventInfo;

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
    void registerMouseDragEvent(BaseWidget* widget);
    
    CForge::GLShader* BackgroundColoredShader;
    CForge::GLShader* TextShader;
    FontFace* fontFace;
private:
    std::vector<BaseWidget*> testBG;
    CForge::RenderDevice* m_renderDevice;

    BaseWidget* focusedWidget = nullptr;
    Eigen::Vector2f focusedClickOffset;
    std::vector<BaseWidget*> m_events_mouseDown;
    std::vector<BaseWidget*> m_events_mouseDrag;
};
