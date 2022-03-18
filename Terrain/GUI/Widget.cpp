#include "Widget.h"
#include "GUI.h"

BaseWidget::BaseWidget ( GUI* rootGUIObject, BaseWidget* parent )
{
    //not sure on what needs to be handed over yet
    //The root object is necessary for handling events
    m_root = rootGUIObject;
    //I assume this could be useful for combined Widgets
    m_parent = parent;
    if (parent != nullptr) level = parent->level + 1;
    else level = 0;
    
    m_x = 0;
    m_y = 0;
    m_width = 0.2;
    m_height = 0.2;
}
void BaseWidget::draw(CForge::RenderDevice* renderDevice)
{
    m_background->render(renderDevice);
    for (auto x : m_children) {
        x->draw(renderDevice);
    }
}
void BaseWidget::onClick ( CForge::Mouse* )
{
    //By default do nothing, have the Widgets overwrite this
    printf("reached here 2");
    return;
}


TestWidget::TestWidget ( GUI* rootGUIObject, BaseWidget* parent ) : BaseWidget(rootGUIObject, parent)
{
    m_background = new WidgetBackgroundColored();
    BackgroundStyle b;
    m_background->init(b, rootGUIObject->BackgroundColoredShader);
    
}
void TestWidget::onClick ( CForge::Mouse* mouse )
{
    m_x = mouse->position()[0] / 720.0f;
    m_y = mouse->position()[1] / 720.0f;
    m_background->setPosition(m_x, m_y);
    printf("reached here");
}

