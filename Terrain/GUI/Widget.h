#pragma once

#include "GUI.h"
#include "WidgetBackground.h"
#include "Font.h"
#include <CForge/Input/Mouse.h>
class GUI;

class BaseWidget {
public:
    int level; //needed for event handling. higher levels receive the event so overlapping lower levels don't trigger
    
    BaseWidget(GUI* rootGUIObject, BaseWidget* parent);
    virtual ~BaseWidget();
    
    virtual void setPosition(float x, float y);
    virtual void changePosition(float dx, float dy);
    virtual void draw(CForge::RenderDevice* renderDevice);

    virtual bool checkHitbox(Eigen::Vector2f pointerPosition);
    
    //Events (need some more thought put into them)
    virtual void focus();
    virtual void focusLost();
    virtual void onClick(mouseEventInfo);
    virtual void onDrag(mouseEventInfo);
    virtual void onKeyPress(char32_t);

    //Basic Getters&Setters
    virtual float getWidth();
    virtual float getHeight();
    virtual Eigen::Vector2f getPosition();
    virtual Eigen::Vector2f getDimension();
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

class TextWidget : public BaseWidget {
public:
    TextWidget(GUI* rootGUIObject, BaseWidget* parent);
    ~TextWidget();
    void setText(std::u32string textString);
    void changeText(char32_t character);
    void setPosition(float x, float y);
    void changePosition(float dx, float dy);
    void setColor(float r, float g, float b);
    void setColor(float color[3]);
    void draw(CForge::RenderDevice* renderDevice);
private:
    std::u32string m_text;
    TextLine* m_pText;
};
