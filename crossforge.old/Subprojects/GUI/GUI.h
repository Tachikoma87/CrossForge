/*****************************************************************************\
*                                                                           *
* File(s): GUI.h and GUI.cpp                                      *
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
#ifndef __CFORGE_GUI_H__
#define __CFORGE_GUI_H__

namespace CForge {
    class BaseWidget;
    class TextWidget;
    class FormWidget;
}
//#include "Widget.h"

#include "WidgetBackground.h"
#include "Font.h"

#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Input/Mouse.h>
#include <CForge/Core/ITListener.hpp>

/*
To be able to define callbacks of different data types without
having to define a million different individual callbacks, a
more abstract structure is needed.

For this, the GUICallbackDatum struct is introduced. The callback
code can then cast the void pointer pData back to its original
type which can be inferred from the Type attribute in form of the enum
GUIInputType (and also, the callback code should already
know which data types it expects).

The GUICallbackObject is what will actually be passed to the
callback function. It includes the FormID of the set of options
it is registered to in order to allow for the same object to
potentially receive multiple callbacks. FormIDs are set during
initialization/creation of the GUI. Likewise, the integer indices
of the Data map are the OptionIDs registered during set up of
the respective form.

The callbacks themselves are handled through CForge's ITListener
and ITCaller interface templates, with the GUICallbackObject data
type.
 */

namespace CForge {
    /**
     * \brief Defines input widget types.
     *
     * This definition is used for forms and their callbacks into program code.
     * It can indicate what the original data type of the pointer in the
     * \ref GUICallbackDatum is. Refer to each input widget's documentation
     * for more details on their data types and what they represent (particularly
     * for the drop down menu input).
     *
     * \ingroup GUI
     */
    enum GUIInputType {
        INPUTTYPE_INT,          ///< Specifies the \ref CForge::InputNumberWidget. Corresponding data type is `int`.
        INPUTTYPE_BOOL,         ///< Specifies the \ref CForge::InputCheckboxWidget. Corresponding data type is `bool`.
        INPUTTYPE_STRING,       ///< Specifies the \ref CForge::InputTextWidget. Corresponding data type is `std::u32string`.
        INPUTTYPE_RANGESLIDER,  ///< Specifies the \ref CForge::InputSliderWidget. Corresponding data type is `float`.
        INPUTTYPE_DROPDOWN      ///< Specifies the \ref CForge::InputDropDownWidget. Corresponding data type is `int`.
    };

    /**
     * \brief Used for abstracting input values.
     *
     * This struct is used to abstract the various different data types of the
     * input widgets. The void pointer `pData` should be casted to match the
     * proper data type (as indicated in the Type attribute). Then it can be
     * dereferenced and used.
     *
     * \ingroup GUI
     */
    struct GUICallbackDatum {
        GUIInputType Type;      ///< Can be used to infer the original data type of the value pointed at.
        void* pData;            ///< Pointer for the value. \sa CForge::LabelWidget::getValue
    };

    /**
     * \brief Used for program callbacks.
     *
     * This struct is passed to program callbacks initiated by the form widget.
     * The callback code can use the FormID to differentiate between different
     * forms. The integer key of the map likewise corresponds to each of the
     * forms options' IDs. Since these IDs are assigned by the developer during
     * initialization/widget creation, they should be well known in the callback
     * code and can be treated with respect to what each option is intended to
     * represent.
     *
     * \ingroup GUI
     */
    struct GUICallbackObject {
        int FormID;             ///< The ID number of the FormWidget that created it.
        std::unordered_map<int, GUICallbackDatum> Data; /**<
                                * \brief A map containing \ref CForge::GUICallbackDatum of all the form's options
                                *
                                * The `int` key is the option's ID, set during its creation.
                                */
    };

    /**
     * \brief Mouse cursor position, used for widget callbacks.
     *
     * This struct is meant to contain the mouse cursor's position in two variants: in window
     * space, and within the widget that was clicked. The mouse coordinates, and by
     * extension the entire coordinates used within the GUI, originate in the top
     * left window corner at point (0,0) and positive values extend towards the
     * bottom right corner.
     *
     * The adjusted position value has it's point of origin (0,0) set to the clicked
     * widget's top left corner instead.
     *
     * \ingroup GUI
     * \sa CForge::GUI::processMouseEvents CForge::BaseWidget::onClick
     */
    struct mouseEventInfo {
        Eigen::Vector2f adjustedPosition;   ///< cursor position adjusted to the clicked widget
        Eigen::Vector2f rawPosition;        ///< cursor position in window space
    };

    /**
     * \brief Class for testing program callbacks.
     *
     * This class can listen to any form's callback and will print the FormID,
     * the option IDs and their respective values to stdout.
     *
     * \ingroup GUI
     */
    class CallbackTestClass : public CForge::ITListener<GUICallbackObject> {
        void listen(const GUICallbackObject Msg) override;
    };

    /**
	* \brief The main GUI class.
    *
    * This main class handles resource loading and organisation for the widgets,
    * orchestrates the callbacks triggered by user input and is the main interaction
    * point for the rest of the program with the GUI.
	*
	* \ingroup GUI
	*/
    class GUI : public CForge::ITListener<char32_t>, public CForge::ITListener<CForge::KeyboardCallback> {
    public:
        /**
         * \brief Please run \ref init after object creation.
         *
         * \ingroup GUI
         */
        GUI();
        ~GUI();

        /**
         * \brief Initializes the GUI.
         *
         * This function should be called after object creation before any
         * other GUI functions are called.
         *
         * This loads required shaders, the used fonts, and wires up the GUI's
         * keyboard input callback.
         *
         * \param[in] pWin      Pointer to the \ref CForge::GLWindow object of the current window the GUI is being initialised in.
         * \sa CForge::GUI::loadFonts
         */
        void init(CForge::GLWindow* pWin);

        /**
         * \brief Draws the GUI into the framebuffer.
         *
         * Iterates over all top level widgets and draws them in their current
         * stacking order into the frame buffer.
         *
         * *Note:* This should be called as the final rendering step. An adequate
         * render pass with the proper frame buffer available should be selected.
         * It will the use simple forward rendering to draw over the scene.
         *
         * \param[in] renderDevice Pointer to the \ref CForge::RenderDevice the rest of the scene has been drawn with.
         * \sa CForge::RenderDevice::activePass
         */
        void render(CForge::RenderDevice* renderDevice);

        /**
         * \brief Creates a \ref CForge::WindowWidget containing a \ref CForge::FormWidget.
         *
         * Returns a reference to the latter that can be used to set up the
         * desired options.
         *
         * The window is managed internally and no further set up is required for
         * the window to show up the next time render() is called. Likewise, clean
         * up is handled by the widgets and the GUI themselves.
         *
         * \param[in] title     The text that the \ref CForge::WindowWidget 's title bar should show.
         * \param[in] FormID    The contained \ref CForge::FormWidget's ID. Important for properly handling the callback.
         * \param[in] applyName The text of the FormWidget's apply button.
         *                      If an empty string is passed, no apply button will be created.
         *                      In that case, all changes to the input widgets will immediately
         *                      trigger a callback. Otherwise, a button with the specified text
         *                      is shown and callbacks will only fire once that is clicked.
         */
        FormWidget* createOptionsWindow(std::u32string title, int FormID, std::u32string applyName = U"Apply");

        /**
         * \brief Creates a plain \ref CForge::TextWidget.
         *
         * Creates a plain text widget with no interaction capabilities. Useful
         * for FPS counters or other information. Needs to be positioned by the
         * developer as it can't be moved during runtime.
         *
         * \sa CForge::TextWidget
         */
        TextWidget* createPlainText();

        /**
         * \brief Creates a \ref CForge::WindowWidget containing a \ref CForge::TextWidget.
         *
         * Just like createOptionsWindow(), this returns only a reference to the
         * child widget. Clean up and other organisation is handled by the GUI class.
         *
         * Opposed to createPlainText(), having a window that can be moved or
         * minimized makes this function more useful if you have large amounts
         * of text you want to display.
         *
         * \param[in] title     The text that the \ref CForge::WindowWidget 's title bar should show.
         * \sa CForge::TextWidget
         */
        TextWidget* createTextWindow(std::u32string title);

        /**
         * \brief Moves/Adds a top level widget to the special popup slot.
         *
         * See \ref m_Popup for a description of what makes popups special.
         *
         * \param[in] widget    The widget to be inserted into the popup slot. Must be a top level widget.
         * \sa m_Popup TopLevelWidgetHandler
         */
        void registerWidgetAsPopup(BaseWidget* widget);

        /**
         * \brief Removes and cleans up the widget currently in the popup slot.
         *
         * This will not only remove them from the slot but also destroy the
         * widget and its children.
         *
         * See \ref m_Popup for a description of what makes popups special.
         *
         * \sa m_Popup
         */
        void unregisterPopup();

        /**
         * \brief Fetch user input and process it.
         *
         * This will fetch input data that can be explicitly acquired (currently
         * only mouse as keyboard uses callbacks) and process it to trigger
         * corresponding callbacks on the widgets affected. Should executed in
         * the program main loop (so, once per frame).
         *
         * \sa processMouseEvents listen(CForge::KeyboardCallback kc)
         */
        void processEvents();

        /** \brief     Register the widget as having click interactivity.
         *  \param[in] widget   Widget that wants to receive mouse clicks.
         *  \sa        registerEvent */
        void registerMouseDownEvent(BaseWidget* widget);

        /** \brief     Register the widget as having mouse dragging interactivity.
         *  \param[in] widget   Widget that can be dragged.
         *  \sa        registerEvent */
        void registerMouseDragEvent(BaseWidget* widget);

        /** \brief     Register the widget as having keyboard interactivity.
         *  \param[in] widget   Widget that wants to receive keyboard inputs.
         *  \sa        registerEvent */
        void registerKeyPressEvent(BaseWidget* widget);

        /** \brief      Passes the entered character on to the focused widget.
         *
         *  In the past, this used to receive the glfwSetCharCallback() more directly,
         *  hence it being a function of its own. Now it just passes the characters on
         *  to the \ref focusedWidget.
         *
         *  \param[in]  codepoint    The 32-bit Unicode representation of the entered character.
         *  \sa         CForge::Keyboard glfwSetCharCallback() */
        void listen(char32_t codepoint) override;

        /** \brief      Receives the entered characters or other keypresses from CForge::Keyboard.
         *
         * This function also checks for key combinations used within the GUI.
         * Currently that is:
         *      - Right CTRL + R: Reset the position of top level widgets.
         *        Useful if a window was moved off-screen.
         *
         *  \param[in]  kc      CForge::Keyboard's callback object.
         *  \sa         CForge::Keyboard */
        void listen(CForge::KeyboardCallback kc) override;

        //stuff accessed by the widgets

        /** \brief      Getter for the GUI's \ref CForge::GLWindow::width */
        uint32_t getWindowWidth();

        /** \brief      Getter for the GUI's \ref CForge::GLWindow::height */
        uint32_t getWindowHeight();

        /** \brief      Shader object for widget backgrounds.
         *              Public so it can be accessed by the background objects.
         *  \sa         CForge::WidgetBackground */
        CForge::GLShader* BackgroundColoredShader;

        /** \brief      Shader object for text rendering.
         *              Public so it can be accessed by the text objects.
         *  \sa         CForge::TextWidget CForge::TextLine */
        CForge::GLShader* TextShader;

        /**
         * \brief Index of loaded fonts for more convenient access.
         *
         * This enum gives more descriptive names to the indices of fontFaces.
         * Font handling isn't optimal at the moment, so this needs to be manually
         * kept in sync with the fonts.
         *
         * The names themselves aren't really specifically defined. The three current
         * entries are used by the current default widgets, but more styles could
         * be added at will.
         *
         * It should be noted that different font sizes are handled as different
         * fonts as well.
         *
         * \sa GUIDefaults.h loadFonts
         */
        enum FontStyles {       //TODO: remember to keep updated when fonts are added/changed
            DEFAULT_FONT = 0,   ///< The default font style, used in most places.
            FONT1_REGULAR = 0,  ///< The regular style of the first loaded font.
            FONT1_BOLD = 1      ///< The bold version of the first loaded font style.
        };

        /** \brief      All the loaded fonts.
         *              Public so it can be accessed by the text objects.
         *  \sa         CForge::TextWidget CForge::TextLine GUIDefaults.h loadFonts */
        std::vector<FontFace*> fontFaces;

    private:

        /** \brief      Internal enumeration of possible event types.
         *  \sa         CForge::registerEvent */
        enum EventType {
            EVENT_CLICK,
            EVENT_DRAG,
            EVENT_KEYPRESS
        };

        /**
         * \brief Registers a widget for the specified event type.
         *
         * Registers the widget to its top level widget as accepting of events of the
         * specified type.
         *
         * The reference to the widget will be added to the lists of the respective
         * TopLevelWidgetHandler in such a way, that it remains sorted by
         * CForge::BaseWidget::m_level in descending order. Because the search for
         * the clicked-on widget will stop after the first viable candidate widget has
         * been found, this effectively prevents larger parent widgets from stealing
         * input meant for their smaller child widgets.
         *
         * \param[in] widget    The widget to register for the events.
         * \param[in] et        The event type the widget should be registered to.
         * \sa registerMouseDownEvent registerMouseDragEvent
         *     registerKeyPressEvent TopLevelWidgetHandler
         */
        void registerEvent(BaseWidget* widget, EventType et);
        //     void unregisterEvent(BaseWidget* widget, EventType et);

        /**
         * \brief Adds a widget as top level widget if it wasn't registered before.
         *
         * Registering events requires a TopLevelWidgetHandler for the widget's root widget
         * to exist. Since events are registered during initialization most of the time,
         * it isn't always possible to register the root manually. This function therefor
         * first checks if the passed in root widget is already registered as top level
         * widget before create a new handler for it.
         *
         * \param[in] widget    The widget to register as top level widget.
         * \sa TopLevelWidgetHandler
         */
        void submitTopLevelWidget(BaseWidget* widget);

        /**
         * \brief Checks mouse position and clicks and triggers respective events.
         *
         * The checks rely on the top level widgets for accurate results and good
         * performance. See TopLevelWidgetHandler for more details.
         *
         * This function triggers both current mouse events (left click and dragging)
         * and the focus event for any type of event.
         *
         * \param[in] widget    The widget to register as top level widget.
         * \sa TopLevelWidgetHandler
         */
        void processMouseEvents(CForge::Mouse* mouse);

        /** \brief      Unused. */
        void processKeyboardEvents(CForge::Keyboard* keyboard);

        /**
         * \brief Loads FreeType and the required fonts.
         *
         * The fonts that should be loaded are specified in GUIDefaults.h. Because
         * the font handling isn't optimal right now, additional fonts need to be
         * added manually to this function for loading.
         *
         * \sa GUIDefaults.h CForge::FontFace
         */
        void loadFonts();

        FT_Library library;                     ///< The FreeType library handler.

        CForge::GLWindow* m_pWin;               ///< Reference to the GLWindow the GUI is shown in.
        BaseWidget* focusedWidget = nullptr;    /**< \brief Reference to the widget that was last clicked.
                                                     Only the focused widget will receive keyboard inputs. **/
        Eigen::Vector2f focusedClickOffset;     /**< \brief The cursor position within a widget when the mouse button was clicked.
                                                     Used to calculate CForge::mouseEventInfo::adjustedPosition **/

        /**
         * \brief Data structure for top level widgets.
         *
         * Top level widgets are a rather important concept of the GUI. Basically,
         * widgets are organized in a tree like structure amongst themselves. The GUI
         * main class however needs to know of these trees so they can be drawn and
         * organised. This is where the top level widgets come into play. The root
         * widgets of these widget trees are registered in the GUI class and wrapped
         * in this struct. It's easier to think of the top level widgets as windows
         * similar to your operating system, as that has been the inspiration for this,
         * although technically any arbitrary widget can be used.
         *
         * Since windows can be moved at will and overlap each other, that stacking order
         * needs to be considered during a number of GUI tasks, like drawing and event
         * handling. It is recorded as the position within \ref m_TopLevelWidgets. When the
         * GUI is told to draw the widgets, that list is traversed in their overlap
         * order.
         *
         * To accomodate this order during event handling, events are bound to
         * the top level widgets. As the GUI class lacks the ability to traverse the
         * widget trees, widgets accepting events need to explicitly register themselves.
         * They are then added to the respective lists of their corresponding
         * TopLevelWidgetHandler.
         * During event handling (in particular, click handling) this allows for overlap
         * to be handled correctly. Instead of checking all widgets in a global list,
         * in a first step only the top level widgets are checked for receiving the click,
         * in the order specified by \ref m_TopLevelWidgets. That way, widgets in the front
         * block those further back from receiving inputs not meant for them. If the
         * click didn't land in the top-most top level widget, the stacking order is
         * also adjusted accordingly.
         * After that first check, the lists of individual widgets are then checked
         * for receiving the click, and callbacks are triggered accordingly.
         */
        struct TopLevelWidgetHandler {
            BaseWidget* pWidget;                        ///< A pointer to the top level widget.
            std::vector<BaseWidget*> eventsMouseDown;   ///< List of all widgets below that top level widget registered for click events.
            std::vector<BaseWidget*> eventsMouseDrag;   ///< List of all widgets below that top level widget registered for mouse drag events.
            std::vector<BaseWidget*> eventsKeyPress;    ///< List of all widgets below that top level widget registered for key press events.
            TopLevelWidgetHandler(BaseWidget* w) {
                pWidget = w;
            }
        };
        std::vector<TopLevelWidgetHandler*> m_TopLevelWidgets;  ///< List of top level widgets. \sa TopLevelWidgetHandler

        /**
         * \brief The popup widget slot.
         *
         * The popup TopLevelWidgetHandler slot is special compared to other top level
         * widgets. If a popup is set, it will always be drawn on top of all other top
         * level widgets and is the only one to receive any clicks.
         *
         * At the current time, once a widget is moved or added to this slot, it cannot
         * be removed apart from destroying it. In other words, registering an existing
         * top level widget as popup and reusing it after popup functionality isn't needed
         * anymore, is currently not possible.
         *
         * \sa registerWidgetAsPopup unregisterPopup TopLevelWidgetHandler
         */
        TopLevelWidgetHandler* m_Popup;
    };

}//name space
#endif
