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
void BaseWidget::setPosition(float x, float y)
{
    m_x = x;
    m_y = y;
}
void BaseWidget::changePosition(float dx, float dy)
{
    m_x += dx;
    m_y += dy;
}
void BaseWidget::draw(CForge::RenderDevice* renderDevice)
{
//     if (m_background != nullptr) m_background->render(renderDevice);
    for (auto x : m_children) {
        x->draw(renderDevice);
    }
}
bool BaseWidget::checkHitbox(Eigen::Vector2f pointerPosition)
{
    return m_x <= pointerPosition[0] && m_y <= pointerPosition[1] &&
        m_x+m_width >= pointerPosition[0] && m_y+m_height >= pointerPosition[1];
}

void BaseWidget::onClick ( CForge::Mouse* )
{
    //By default do nothing, have the Widgets overwrite this
    printf("reached here 2");
    return;
}


TestWidget::TestWidget ( GUI* rootGUIObject, BaseWidget* parent ) : BaseWidget(rootGUIObject, parent)
{
//     m_background = new WidgetBackgroundColored();
//     BackgroundStyle b;
//     m_background->init(b, rootGUIObject->BackgroundColoredShader);

    TextWidget* textTest = new TextWidget(rootGUIObject, this);
    textTest->setText(U"Zeile 1");
    m_children.push_back(textTest);
    textTest = new TextWidget(rootGUIObject, this);
    textTest->changePosition(0, 50);
    textTest->setText(U"und Zeile 2.");
    m_children.push_back(textTest);

    //TODO: size reporting for text changes
    m_width = 200;
    m_height = 100;
}
void TestWidget::onClick ( CForge::Mouse* mouse )
{
    printf("reached here");
    changePosition(mouse->movement()[0],mouse->movement()[1]);
    for (auto x : m_children) {
        x->changePosition(mouse->movement()[0],mouse->movement()[1]);
    }
}

TextWidget::TextWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
{
    m_pText = new TextLine;
    m_pText->init(m_root->fontFace, m_root->TextShader);
}
void TextWidget::draw(CForge::RenderDevice* renderDevice)
{
    //until I've revamped the WidgetBackground interface to fit the TextLine
    //object, use that directly.
    //TODO: can it even be made to fit an agnostic interface? We do need
    //      the function to change text afterall.
    m_pText->render(renderDevice);
    for (auto x : m_children) {
        x->draw(renderDevice);
    }
}
void TextWidget::setText(std::u32string textString)
{
    m_text = textString;
    m_pText->setText(textString);
}
void TextWidget::changeText(char32_t character)
{
    //can be expanded in the future to support a text cursor
    //once it's clear how the key code to ascii/unicode mapping will happen
    //TODO: for now only takes unicode as input for testing
    switch (character) {
        case 8:     //Backspace: delete last chacter from string
            m_text.pop_back();
            break;
        default:     //treat as normal character
            m_text.push_back(character);
            break;
    }
    m_pText->setText(m_text);
}
void TextWidget::setPosition(float x, float y)
{
    m_x = x;
    m_y = y;
    m_pText->setPosition(x, y);
}

void TextWidget::changePosition(float dx, float dy)
{
    m_x += dx;
    m_y += dy;
    m_pText->setPosition(m_x, m_y);
}


