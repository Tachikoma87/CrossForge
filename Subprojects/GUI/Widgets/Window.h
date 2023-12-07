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
* MIT License without any warranty or guaranty to work properly.            *
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

    /**
     * \brief Button within the WindowWidget's title bar to minimize the window.
     * \ingroup GUI
     */
    class WindowWidget_MinimizeButton : public TextWidget {
    public:
        WindowWidget_MinimizeButton(WindowWidget* window, GUI* rootGUIObject, BaseWidget* parent);
        ~WindowWidget_MinimizeButton();

        /**
         * \brief Mouse click callback handler.
         *
         * Used to toggle the WindowWidget's minimized state.
         *
         * \param[in] mouse     Mouse cursor position.
         */
        void onClick(mouseEventInfo) override;

    private:
        void changeAppearance();
        bool m_minimized;
        WindowWidget* m_window;
    };

    /**
     * \brief The title bar of the WindowWidget.
     * \ingroup GUI
     */
    class WindowWidget_Header : public BaseWidget {
    public:
        /**
         * \brief Initialises the widget.
         *
         * Creates background, border, a CForge::TextWidget for the title, and the minimize button.
         * As with practically all GUI components, clean up is autoatically handled by the destructor.
         *
         * \param[in] title     The text that should be displayed in the title bar.
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         */
        WindowWidget_Header(std::u32string title, GUI* rootGUIObject, WindowWidget* parent);
        ~WindowWidget_Header();

        /**
         * \brief Reposition the minimize button.
         *
         * If the window state was changed ([un-]minimized), the minimize button will change
         * its displayed text and the title bar will be resized to either the minimal width
         * to fit the title, or to match the content width. This function changes the title
         * bar's width and repositions the button in accordance.
         */
        void updateChildPositions();

        /**
         * \brief Mouse drag callback handler.
         *
         * Used to let the user move the WindowWidget around.
         *
         * \param[in] mouse     Mouse cursor position.
         */
        void onDrag(mouseEventInfo mouse) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

    private:
        WindowWidget* m_window;
        TextWidget* m_title;
        WindowWidget_MinimizeButton* m_button;
    };

    /**
     * \brief A widget representing a movable, minimizable window.
     *
     * In case the user moved the window off-screen, the position of all top level
     * widgets (which windows most likely are) can be reset by pressing `R` while
     * holding the *right* CTRL key.
     *
     * \ingroup GUI
     */
    class WindowWidget : public BaseWidget {
    public:
        /**
         * \brief Initialises the widget.
         *
         * Creates background and border, and the title bar and the minimize button.
         *
         * \param[in] title     The text that should be displayed in the title bar.
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         */
        WindowWidget(std::u32string title, GUI* rootGUIObject, BaseWidget* parent);
        ~WindowWidget();

        /**
         * \brief Set the widget that should provide the window's content.
         *
         * That is, the body of a window that is not currently minimized.
         * Usually something like a CForge::FormWidget or CForge::TextWidget.
         *
         * It is important to note that the content widget should have the WindowWidget
         * set as its parent widget. Thus, the proper way to call this function
         * looks something like this:
         *      1) create the WindowWidget instance.
         *      2) create an instance of the content widget, with the WindowWidget
         *         set as its parent.
         *      3) call this function on the WindowWidget.
         *
         * Another thing to note is that the content widget will be freed by the
         * WindowWidget upon its destruction.
         *
         * \param[in] content   Reference to the content widget.
         */
        void setContentWidget(BaseWidget* content);

        /**
         * \brief Minimize or expand the window.
         *
         * If the parameter is `true`, the window will be minimized, otherwise it
         * will be expanded to its full size again.
         *
         * \param[in] state     Whether the window should be minimized.
         */
        void minimize(bool state);

        /** \brief Getter for whether the window is currently minimized. */
        bool isMinimized();

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

    private:
        bool m_minimized;                   ///< Whether the widget is currently minimized.
        WindowWidget_Header* m_header;      ///< Reference to the title bar widget.
        WidgetBackgroundBorder* m_border;   ///< Reference to the widget's border object.
        BaseWidget* m_content;              ///< Reference to the content widget.
    };

}//name space
#endif
