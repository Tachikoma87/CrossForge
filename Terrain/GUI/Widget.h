#pragma once

#include "WidgetBackground.h"
#include <CForge/Input/Mouse.h>
class GUI;

class BaseWidget {
public:
    int level; //needed for event handling. higher levels receive the event so overlapping lower levels don't trigger
    
    BaseWidget(GUI* rootGUIObject, BaseWidget* parent);
    
    void draw(CForge::RenderDevice* renderDevice);
    
    //Events (need some more thought put into them)
    virtual void onClick(CForge::Mouse*);
protected:
    GUI* m_root;
    BaseWidget* m_parent;
    std::vector<BaseWidget*> m_children;
    WidgetBackground* m_background;
    float m_x;     //top left point
    float m_y;
    float m_width; 
    float m_height;
};

class TestWidget : public BaseWidget {
public:
    TestWidget(GUI* rootGUIObject, BaseWidget* parent);
    void onClick(CForge::Mouse* mouse);
};

class TextWidget : public BaseWidget {
public:
    TextWidget(GUI* rootGUIObject, BaseWidget* parent);
    void setText(std::string textString);
};
