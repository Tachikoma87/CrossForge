/*****************************************************************************\
*                                                                           *
* File(s): Widget.h and Widget.cpp                                      *
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
#ifndef __CFORGE_WIDGET_H__
#define __CFORGE_WIDGET_H__


#include "GUI.h"
#include "GUIDefaults.h"
#include "WidgetBackground.h"
#include "Font.h"
#include <CForge/Input/Mouse.h>

namespace CForge {

    /**
     * \brief The BaseWidget interface class.
     *
     * This is the basic interface class common to all widgets.
     *
     * \ingroup GUI
     */
    class BaseWidget {
    public:

        /**
         * \brief Initialises the widget.
         *
         * Saves a reference to both the widget's parent (if applicable)
         * and the main GUI class. Then aligns the widget's position to its
         * parent.
         *
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         */
        BaseWidget(GUI* rootGUIObject, BaseWidget* parent);
        virtual ~BaseWidget();

        //Positioning and Layout

        /**
         * \brief Sets the widget position in window space.
         *
         * Any widgets overriding this method should propagate the call to this
         * function to any child widgets, and update any CForge::WidgetBackground
         * objects besides #m_background (which is already handled by this interface
         * class).
         */
        virtual void setPosition(float x, float y);

        /**
         * \brief Adjusts the position relative to the widget's current position.
         *
         * Any widgets overriding this method should propagate the call to this
         * function to any child widgets, and update any CForge::WidgetBackground
         * objects besides #m_background (which is already handled by this interface
         * class).
         */
        virtual void changePosition(float dx, float dy);

        /**
         * \brief Tells the widget it should recalculate its layout.
         *
         * If a child widget's size or layout changed (eg. because of some
         * user input), it should call its parent's updateLayout method to
         * signal it that a change happened and that the parent's layout
         * now might require updating to accomodate those changes.
         *
         * What usually happens is that some widget's text and thus width
         * change. In order to not overflow the containing widget or leave
         * awkward unused space, it needs to make its parent widget aware
         * of this. The parent widget should then in turn tell its own parent
         * to do the same if it couldn't fully "absorb" the layout changes.
         * The updateLayout calls should then cascade all the way to the top
         * level widget.
         */
        virtual void updateLayout();

        /**
         * \brief Initiate rendering of the widget and it's children.
         *
         * In this function, everything that needs to be displayed should
         * have its draw call issued. That includes:
         *  - any CForge::WidgetBackground objects
         *  - any child widgets
         *  - and in the case of the TextWidget, any CForge::TextLine objects.
         *
         * \param[in] pRDev Pointer to the CForge::RenderDevice used for drawing the GUI/Scene.
         */
        virtual void draw(CForge::RenderDevice* renderDevice);


        //For event handling

        /** \brief Getter for the widget's level. That is, how far away it is from
         *         its top level widget. */
        virtual int getLevel();

        /** \brief Recursively find the widget's top level ancestor. */
        virtual BaseWidget* getTopWidget();

        /** \brief Check if the specified cursor position (or any other point in
         *         window space) lies within this widget.
         *  \param[in] pointerPosition Position of the mouse cursor during a click.  */
        virtual bool checkHitbox(Eigen::Vector2f pointerPosition);


        //Events (need some more thought put into them)

        /** \brief Tells a widget one of its child input widgets had a change of
         *         value (eg. through user input).
         *  \param[in] child Reference to the widget that had its value changed. */
        virtual void childValueChanged(BaseWidget* child);

        /** \brief Triggered when a widget was clicked and subsequently received focus.
         *
         * Unlike BaseWidget::onClick, this does not require the widget to be registered for
         * click events. Registration for any event type will trigger the focus. */
        virtual void focus();

        /** \brief Triggered when the widget lost its focus. */
        virtual void focusLost();

        /** \brief Triggered when the widget is registered for click events and was
         *         clicked on.
         *  \sa CForge::GUI::registerMouseDownEvent */
        virtual void onClick(mouseEventInfo);

        /** \brief Triggered when the widget is registered for drag events and the
         *         click is held down.
         *  \sa CForge::GUI::registerMouseDragEvent */
        virtual void onDrag(mouseEventInfo);

        /** \brief Triggered when the widget is registered for keyboard events, is
         *         currently focused and keys are pressed.
         *  \sa CForge::GUI::registerKeyPressEvent */
        virtual void onKeyPress(char32_t);


        //Basic Getters&Setters

        /** \brief Getter for the widget's width. */
        virtual float getWidth();

        /** \brief Getter for the widget's height. */
        virtual float getHeight();

        /** \brief Getter for the widget's position (x, y). */
        virtual Eigen::Vector2f getPosition();

        /** \brief Getter for the widget's size (width, height). */
        virtual Eigen::Vector2f getDimension();
    protected:
        GUI* m_root;                            ///< A reference to the main GUI class instance.
        BaseWidget* m_parent;                   ///< A reference to the widget's parent widget.
        std::vector<BaseWidget*> m_children;    ///< A list of child widgets. Unused by all currently existing widgets.
        WidgetBackground* m_background;         ///< A reference to a WidgetBackground object.
        //level is needed for event handling. higher levels receive the event so overlapping lower levels don't trigger
        int m_level;                            ///< Distance of the widget from its top level widget.
        float m_x;                              ///< Window space X coordinate.
        float m_y;                              ///< Window space Y coordinate.
        float m_width;                          ///< Widget width.
        float m_height;                         ///< Widget height.
    };

    /**
     * \brief A widget with text displayig capabilities.
     *
     * Based on the common BaseWidget and expanded to include methods to
     * easily get text on the screen. Multi-line text is possible.
     *
     * \ingroup GUI
     */
    class TextWidget : public BaseWidget {
    public:

        /**
         * \brief Initialises the widget.
         *
         * Calls BaseWidget() and additionally loads the default font style
         * (`FontStyle1`).
         *
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         */
        TextWidget(GUI* rootGUIObject, BaseWidget* parent);

        /**
         * \brief Destructor for the widget.
         *
         * Cleans up any CForge::TextLine objects that were in use.
         */
        ~TextWidget();

        /**
         * \brief Set the specified text to be displayed.
         *
         * Multi-line Text is possible by using newline characters (``U'\n'``)
         * in the string.
         *
         * \param[in] textString The string to be displayed.
         */
        void setText(std::u32string textString);

        /** \brief Getter for the widget's displayed text.
         *  \todo Not sure if this properly supports multi-line */
        std::u32string getText();

        /**
         * \brief Appends the given character to the displayed text string.
         *
         * Always appends to the back currently. Removing characters is
         * done by passing ``U'\b'`` or `8`, the ASCII/Unicode representation
         * of backspace.
         *
         * \param[in] textString The string to be displayed.
         */
        void changeText(char32_t character);
        //     void changeFont(FontFace* newFont);

        /**
         * \brief Change the font to the specified new one.
         *
         * This replaces the CForge::FontFace reference in use with the
         * one identified by parameter. Note that this doesn't immediately
         * redraw the displayed text. It will only take effect the next
         * time that string is changed.
         *
         * \param[in] style The identifier of the font that should be used.
         */
        void changeFont(GUI::FontStyles style);
        //     void setPosition(float x, float y);
        void changePosition(float dx, float dy);

        /** \brief Changes the text colour. Range 0.0 to 1.0 */
        void setColor(float r, float g, float b);

        /** \brief Changes the text colour. Accepts an array for convenience. */
        void setColor(float color[3]);

        /**
         * \brief Describes multi-line text's alignment.
         *
         * Specifies the alignment/justification of the individual lines of
         * multi-line text.
         */
        enum TextAlign {
            ALIGN_LEFT,     ///< Left-align the text. Default.
            ALIGN_CENTER,   ///< Center-align the text.
            ALIGN_RIGHT     ///< Right-align the text.
        };

        /**
         * \brief Set the new text alignment.
         *
         * Like TextWidget::changeFont, this only takes
         * effect the next time the text is updated.
         *
         * \param[in] align The new text alignment.
         */
        void setTextAlign(TextAlign align);     //for now only applied at next text change

        void draw(CForge::RenderDevice* renderDevice);
    private:
        std::u32string m_text;              ///< The currently displayed text.
        //     TextLine* m_pText;
        std::vector<TextLine*> m_TextLines; ///< The CForge::TextLine objects, that actually display the text.
        int m_padding;                      ///< The padding added around the text. Configured in CForge::WidgetStyle
        GUI::FontStyles m_FontStyle;        ///< Identifier of the currently used font.
        TextAlign m_Align;                  ///< Current text alignment.
    };

}//Widget
#endif
