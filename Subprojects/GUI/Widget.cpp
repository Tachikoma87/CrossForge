#include "Widget.h"

namespace CForge {

    BaseWidget::BaseWidget(GUI* rootGUIObject, BaseWidget* parent)
    {
        //not sure on what needs to be handed over yet
        //The root object is necessary for handling events
        m_root = rootGUIObject;
        //I assume this could be useful for combined Widgets
        m_parent = parent;
        if (parent != nullptr) {
            m_level = parent->getLevel() + 1;
            m_x = parent->getPosition()[0];
            m_y = parent->getPosition()[1];
        }
        else {
            m_level = 0;
            m_x = 0;
            m_y = 0;
        }
        m_width = 20;
        m_height = 20;

        m_background = nullptr;
    }
    BaseWidget::~BaseWidget()
    {
        for (auto x : m_children) {
            delete x;
        }
    }

    void BaseWidget::setPosition(float x, float y)
    {
        //not the most straightforward way to do it, but works better with the derived widgets
        float dx = x - m_x;
        float dy = y - m_y;
        changePosition(dx, dy);
    }
    void BaseWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        if (m_background != nullptr) m_background->updatePosition();
    }
    void BaseWidget::updateLayout()
    {
        //This method should be called by child widgets if their size or relative position changes
        //(like eg. the number input)
        //since no widget seems to actually use m_children at this point in time,
        //no default default will be provided yet.
        return;
    }
    void BaseWidget::draw(CForge::RenderDevice* renderDevice)
    {
        if (m_background != nullptr) m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        for (auto x : m_children) {
            x->draw(renderDevice);
        }
    }
    int BaseWidget::getLevel()
    {
        return m_level;
    }
    BaseWidget* BaseWidget::getTopWidget()
    {
        if (m_parent == nullptr) {
            return this;
        }
        else {
            return m_parent->getTopWidget();
        }
    }
    bool BaseWidget::checkHitbox(Eigen::Vector2f pointerPosition)
    {
        return m_x <= pointerPosition[0] && m_y <= pointerPosition[1] &&
            m_x + m_width >= pointerPosition[0] && m_y + m_height >= pointerPosition[1];
    }

    void BaseWidget::childValueChanged(BaseWidget* child)
    {
        //the value of a child input widget was changed.
        //pass it on to the parent until some widget does something with it.
        if (m_parent != nullptr) m_parent->childValueChanged(child);
    }
    void BaseWidget::focus()
    {
        return;
    }
    void BaseWidget::focusLost()
    {
        return;
    }
    void BaseWidget::onClick(mouseEventInfo)
    {
        //By default do nothing, have the Widgets overwrite this
        return;
    }
    void BaseWidget::onDrag(mouseEventInfo)
    {
        return;
    }
    void BaseWidget::onKeyPress(char32_t)
    {
        return;
    }

    float BaseWidget::getWidth()
    {
        return m_width;
    }
    float BaseWidget::getHeight()
    {
        return m_height;
    }
    Eigen::Vector2f BaseWidget::getPosition()
    {
        return Eigen::Vector2f(m_x, m_y);
    }
    Eigen::Vector2f BaseWidget::getDimension()
    {
        return Eigen::Vector2f(m_width, m_height);
    }





    TextWidget::TextWidget(GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        //initialize with default font style
        m_FontStyle = GUI::DEFAULT_FONT;
        m_Align = ALIGN_LEFT;
        FontStyle1 defaultFont;
        //     m_pText = new TextLine;
        //     m_pText->init(m_root->fontFaces[m_FontStyle], m_root->TextShader);
        setColor(defaultFont.FontColor);
        //     m_pText->setRenderSize(m_root->getWindowWidth(), m_root->getWindowHeight());
        WidgetStyle defaults;
        m_padding = defaults.TextPadding;
        //     m_pText->setPosition(m_padding, m_padding);
        m_height = /*m_pText->getTextSize()*/ +2 * m_padding;
    }
    TextWidget::~TextWidget()
    {
        //     delete m_pText;
        for (auto x : m_TextLines) delete x;
    }
    void TextWidget::draw(CForge::RenderDevice* renderDevice)
    {
        if (m_background != nullptr) m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        //     m_pText->render(renderDevice);
        for (auto x : m_TextLines) {
            x->render(renderDevice);
        }
    }
    /*
    void TextWidget::changeFont(FontFace* newFont)
    {
        m_pText->changeFont(newFont);
    }*/
    void TextWidget::changeFont(GUI::FontStyles style)
    {
        m_FontStyle = style;
        //     m_pText->changeFont(m_root->fontFaces[style]);
        for (auto x : m_TextLines) {
            x->changeFont(m_root->fontFaces[style]);
        }
    }
    void TextWidget::setText(std::u32string textString)
    {
        m_text = textString;

        //calculate the width of the string to be rendered
        //this could be done during rendering and passing an information object
        //or something like that, saving one iteration over the string.
        //However, having a seperate function for it has some benefits
        //(eg. it could be expanded to cut-off/break up long strings)
    //     m_width = m_pText->computeStringWidth(textString) + 2*m_padding;
        std::vector<std::u32string> lines;
        int maxStringWidth = m_root->fontFaces[m_FontStyle]->computeStringWidthMultiline(textString, &lines);
        while (m_TextLines.size() > lines.size()) {
            TextLine* tl = m_TextLines.back();
            delete tl;
            m_TextLines.pop_back();
        }
        while (m_TextLines.size() < lines.size()) {
            //TODO: new Lines currently won't have the same text color if it's different from the default
            TextLine* tl = new TextLine;
            tl->init(m_root->fontFaces[m_FontStyle], m_root->TextShader);
            tl->setRenderSize(m_root->getWindowWidth(), m_root->getWindowHeight());
            tl->setPosition(m_x + m_padding, m_y + m_padding);
            m_TextLines.push_back(tl);
        }
        m_height = m_padding;
        WidgetStyle defaults;
        for (int i = 0; i < lines.size(); i++) {
            if (i > 0)
                m_height += defaults.TextLineGap;
            m_TextLines[i]->setText(lines[i]);
            float x = m_x + m_padding;
            switch (m_Align) {
            case ALIGN_RIGHT:
                x += maxStringWidth - m_TextLines[i]->getTextWidth();
                break;
            case ALIGN_CENTER:
                x += 0.5 * (maxStringWidth - m_TextLines[i]->getTextWidth());
                break;
            default:
                break;
            }
            m_TextLines[i]->setPosition(x, m_y + m_height);
            m_height += m_TextLines[i]->getTextSize();
        }

        m_width = maxStringWidth + 2 * m_padding;
        m_height += m_padding;
        if (m_background != nullptr) m_background->updateSize();

        //     m_pText->setText(textString);
    }
    std::u32string TextWidget::getText()
    {
        return m_text;
    }
    void TextWidget::changeText(char32_t character)
    {
        //can be expanded in the future to support a text cursor
        //once it's clear how the key code to ascii/unicode mapping will happen
        //TODO: for now only takes unicode as input for testing
        switch (character) {
        case 8:     //Backspace: delete last chacter from string
            if (m_text.length() > 0) m_text.pop_back();
            break;
        default:     //treat as normal character
            m_text.push_back(character);
            break;
        }
        setText(m_text);
    }
    // void TextWidget::setPosition(float x, float y)
    // {
    //     m_x = x;
    //     m_y = y;
    // //     m_pText->setPosition(x + m_padding, y + m_padding);
    //     for (auto a : m_TextLines) {
    //         a->setPosition(x + m_padding, y + m_padding);
    //     }
    //     if (m_background != nullptr) m_background->updatePosition();
    // }

    void TextWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        //     m_pText->setPosition(m_x + m_padding, m_y + m_padding);
        for (auto a : m_TextLines) {
            a->setPosition(m_x + m_padding, m_y + m_padding);
        }
        WidgetStyle defaults;
        int a = m_padding;
        for (int i = 0; i < m_TextLines.size(); i++) {
            if (i > 0)
                a += defaults.TextLineGap;
            float x = m_x + m_padding;
            switch (m_Align) {
            case ALIGN_RIGHT:
                x += m_width - 2 * m_padding - m_TextLines[i]->getTextWidth();
                break;
            case ALIGN_CENTER:
                x += 0.5 * (m_width - m_TextLines[i]->getTextWidth()) - m_padding;
                break;
            default:
                break;
            }
            m_TextLines[i]->setPosition(x, m_y + a);
            a += m_TextLines[i]->getTextSize();
        }
        if (m_background != nullptr) m_background->updatePosition();
    }
    void TextWidget::setColor(float r, float g, float b)
    {
        //     m_pText->setColor(r, g, b);
        for (auto x : m_TextLines) {
            x->setColor(r, g, b);
        }
    }
    void TextWidget::setColor(float color[3])
    {
        //     m_pText->setColor(color[0], color[1], color[2]);
        for (auto x : m_TextLines) {
            x->setColor(color[0], color[1], color[2]);
        }
    }
    void TextWidget::setTextAlign(TextWidget::TextAlign align)
    {
        m_Align = align;
    }

}//name space