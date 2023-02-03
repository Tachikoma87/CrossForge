#include "Window.h"

namespace CForge {

    WindowWidget_MinimizeButton::WindowWidget_MinimizeButton(WindowWidget* window, GUI* rootGUIObject, BaseWidget* parent) : TextWidget(rootGUIObject, parent)
    {
        m_window = window;
        m_minimized = m_window->isMinimized();
        changeAppearance();
        rootGUIObject->registerMouseDownEvent(this);
        m_background = new WidgetBackgroundBorder(this, m_root);
    }
    WindowWidget_MinimizeButton::~WindowWidget_MinimizeButton()
    {
        delete m_background;
    }
    void WindowWidget_MinimizeButton::onClick(mouseEventInfo)
    {
        m_minimized = !m_minimized;
        changeAppearance();
        m_window->minimize(m_minimized);
        m_parent->updateLayout();
    }
    void WindowWidget_MinimizeButton::changeAppearance()
    {
        if (m_minimized) {
            setText(U"+");
        }
        else {
            setText(U"_");
        }
    }


    WindowWidget_Header::WindowWidget_Header(std::u32string title, GUI* rootGUIObject, WindowWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_window = parent;
        m_title = new TextWidget(rootGUIObject, this);
        m_title->changeFont(GUI::FONT1_BOLD);
        m_title->setText(title);
        m_button = new WindowWidget_MinimizeButton(m_window, rootGUIObject, this);
        m_height = std::max(m_title->getHeight(), m_button->getHeight());
        updateChildPositions();
        m_background = new WidgetBackgroundBorder(this, m_root);
        m_root->registerMouseDragEvent(this);
    }
    WindowWidget_Header::~WindowWidget_Header()
    {
        delete m_title;
        delete m_button;
        delete m_background;
    }
    void WindowWidget_Header::updateChildPositions()
    {
        WidgetStyle defaults;
        float minWidth = m_title->getWidth() + m_button->getWidth() + defaults.WithinWidgetPadding;
        if (m_window->isMinimized()) {
            m_width = minWidth;
        }
        else {
            m_width = std::max(m_window->getWidth(), minWidth);
        }
        float x = m_x + m_width - m_button->getWidth();
        m_button->setPosition(x, m_y);
        if (m_background != nullptr) m_background->updateSize();
    }
    void WindowWidget_Header::updateLayout()
    {
        updateChildPositions();
        //does not need to propagate to the WindowWidget because the size difference
        //is compensated by this widget already.
    //     m_parent->updateLayout();
    }
    void WindowWidget_Header::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        m_title->changePosition(dx, dy);
        m_button->changePosition(dx, dy);
        if (m_background != nullptr) m_background->updatePosition();
    }
    void WindowWidget_Header::draw(CForge::RenderDevice* renderDevice)
    {
        if (m_background != nullptr) m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_title->draw(renderDevice);
        m_button->draw(renderDevice);
    }
    void WindowWidget_Header::onDrag(mouseEventInfo mouse)
    {
        m_parent->setPosition(mouse.adjustedPosition[0], mouse.adjustedPosition[1]);
    }


    WindowWidget::WindowWidget(std::u32string title, GUI* rootGUIObject, BaseWidget* parent) : BaseWidget(rootGUIObject, parent)
    {
        m_minimized = true;
        m_header = new WindowWidget_Header(title, rootGUIObject, this);
        m_height = m_header->getHeight();
        m_width = m_header->getWidth();
        m_background = new WidgetBackgroundColored(this, m_root);
        m_border = new WidgetBackgroundBorder(this, m_root);
        m_border->setLineWidth(2);
    }
    WindowWidget::~WindowWidget()
    {
        delete m_header;
        delete m_background;
        delete m_border;
        delete m_content;
    }

    /*
     NOTE: The content widget should be set up with the Window as its parent!
     Also, the content will be freed by the window's destructor

     So the proper course of action looks like:
        1) create WindowWidget
        2) create content widget, set WindowWidget as parent
        3) call setContentWidget on the Window
     */
    void WindowWidget::setContentWidget(BaseWidget* content)
    {
        m_content = content;
        WidgetStyle defaults;
        m_content->changePosition(defaults.WithinWidgetPadding, m_header->getHeight() + defaults.WithinWidgetPadding);
        updateLayout();
    }
    void WindowWidget::minimize(bool state)
    {
        m_minimized = state;
        m_header->updateChildPositions();
        if (m_minimized) {
            m_height = m_header->getHeight();
            m_width = m_header->getWidth();
        }
        else {
            if (m_content != nullptr) {
                WidgetStyle defaults;
                m_height = m_header->getHeight() + m_content->getHeight() + 2 * defaults.WithinWidgetPadding;
                m_width = std::max(m_header->getWidth(), m_content->getWidth() + 2 * defaults.WithinWidgetPadding);
            }
        }
        if (m_parent != nullptr) m_parent->updateLayout();
        m_background->updateSize();
        m_border->updateSize();
    }
    bool WindowWidget::isMinimized()
    {
        return m_minimized;
    }
    void WindowWidget::updateLayout()
    {
        //only handle content size changes, since the header takes care of its
        //changes on its own.
        if (m_content != nullptr && !m_minimized) {
            WidgetStyle defaults;
            m_width = m_content->getWidth() + 2 * defaults.WithinWidgetPadding;
            m_height = m_header->getHeight() + m_content->getHeight() + 2 * defaults.WithinWidgetPadding;
            m_header->updateChildPositions();
            m_width = m_header->getWidth();
            m_background->updateSize();
            m_border->updateSize();
            if (m_parent != nullptr) m_parent->updateLayout();
        }
    }
    void WindowWidget::changePosition(float dx, float dy)
    {
        m_x += dx;
        m_y += dy;
        m_header->changePosition(dx, dy);
        if (m_content != nullptr) m_content->changePosition(dx, dy);
        m_background->updatePosition();
        m_border->updatePosition();
    }
    void WindowWidget::draw(CForge::RenderDevice* renderDevice)
    {
        m_background->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_border->render(renderDevice, Eigen::Quaternionf::Identity(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Ones());
        m_header->draw(renderDevice);
        if (m_content != nullptr && !m_minimized) m_content->draw(renderDevice);
    }

}//name space