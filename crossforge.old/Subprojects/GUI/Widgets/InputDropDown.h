/*****************************************************************************\
*                                                                           *
* File(s): InputDropdown.h and InputDropdown.cpp                                      *
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
#ifndef __CFORGE_INPUTDROPDOWN_H__
#define __CFORGE_INPUTDROPDOWN_H__

#include "../Widget.h"
#include "../GUI.h"

namespace CForge {

    class InputDropDownWidget;

    /**
     * \brief A single entry in a drop down menu.
     * \ingroup GUI
     */
    class InputDropDownWidget_PopupEntry : public TextWidget {
    public:
        /**
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         * \param[in] dropdown  A reference to the InputDropDownWidget that spawned this entry.
         */
        InputDropDownWidget_PopupEntry(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown);
        /**
         * \brief Mouse click callback handler.
         *
         * Used to set the InputDropDownWidget's value to this option and close the popup.
         *
         * \sa InputDropDownWidget::closePopup
         */
        void onClick(mouseEventInfo) override;
        /**
         * \brief Sets display text and return value of this entry.
         *
         * If active is true, this entry will be highlighted in the drop down
         * menu as the currently active selection.
         *
         * \param[in] id        Value if this option is selected.
         * \param[in] text      This entry's display text.
         * \param[in] active    Whether this option is the currently selected one.
         */
        void setOption(int id, std::u32string text, bool active = false);
    private:
        int m_OptionID;                     ///< Return value if this option is selected.
        InputDropDownWidget* m_pDropdown;   ///< Reference to the InputDropDownWidget which drop down selection has spawned this entry.
    };

    /**
     * \brief The drop down menu box containing the options.
     * \ingroup GUI
     */
    class InputDropDownWidget_PopupBox : public BaseWidget {
    public:
        /**
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         * \param[in] dropdown  A reference to the InputDropDownWidget that spawned this entry.
         */
        InputDropDownWidget_PopupBox(GUI* rootGUIObject, BaseWidget* parent, InputDropDownWidget* dropdown);
        ~InputDropDownWidget_PopupBox();
        /**
         * \brief Create entries in the popup for the given options.
         *
         * In the drop down list, create option entries for the options defined
         * by optionMap. Its integer key is the input's expected value if that
         * option is selected, the string is its display text.
         *
         * \param[in] optionMap Map of options and their values.
         */
        void createEntries(std::map<int, std::u32string> optionMap);

        void changePosition(float dx, float dy) override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        std::vector<InputDropDownWidget_PopupEntry*> m_entries; ///< List holding the individual drop down entries.
        InputDropDownWidget* m_pDropdown;   ///< Reference to the InputDropDownWidget which spawned this drop down menu.
        WidgetBackgroundBorder m_border;    ///< additional border object.
    };

    /**
     * \brief The drop down popup widget.
     *
     * When the InputDropDownWidget is clicked, a drop down menu containing
     * all the available options appears. This utilizes the CForge::GUI's
     * popup mechanism to have it catch all input and be drawn on top.
     *
     * This widget wraps the actual drop down menu (InputDropDownWidget_PopupBox)
     * to rectify CForge::GUI's lack of a dedicated "clicked outside of the
     * popup" callback. Its size covers the whole application window and
     * any click not caught by the individual entries will be received by
     * this widget and close the popup without further action.
     *
     * \ingroup GUI
     */
    class InputDropDownWidget_Popup : public BaseWidget {
    public:
        /** Despite the parent parameter, the BaseWidget constructor will be
         *  called without it in order to faciliate this widget as top level
         *  widget and be able to use the popup mechanism. */
        InputDropDownWidget_Popup(GUI* rootGUIObject, InputDropDownWidget* parent);
        ~InputDropDownWidget_Popup();

        /**
         * \brief Create entries in the popup for the given options.
         *
         * In the drop down list, create option entries for the options defined
         * by optionMap. Its integer key is the input's expected value if that
         * option is selected, the string is its display text.
         *
         * Passed on to InputDropDownWidget_PopupBox::createEntries.
         *
         * \param[in] optionMap Map of options and their values.
         */
        void createEntries(std::map<int, std::u32string> optionMap);

        /**
         * \brief Mouse click callback handler.
         *
         * Used to close the popup without changing the value.
         *
         * \sa InputDropDownWidget::closePopup
         */
        void onClick(mouseEventInfo mouse) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        InputDropDownWidget* m_pDropdown;       ///< Reference to the InputDropDownWidget which spawned this drop down menu.
        InputDropDownWidget_PopupBox* m_box;    ///< The popup box actually presenting the options to the user.
    };

    /**
     * \brief An input widget realising a drop down menu.
     *
     * Implements an input mechanic commonly called a drop down menu.
     * That is, it indicates the currently selected value and, only
     * upon clicking the widget, will show a list of all predefined
     * possible values for the user to select one.
     *
     * \ingroup GUI
     */
    class InputDropDownWidget : public BaseWidget {
    public:
        InputDropDownWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputDropDownWidget();

        /**
         * \brief Sets the widget's value.
         * \param[in] value     The new value.
         */
        void setValue(int value);
        /**
         * \brief Gets the widget's value.
         */
        int getValue();
        /**
         * \brief Sets what options should be available.
         *
         * The optionMap is defined as follows:
         *      - Integer key: the value of this widget if that option is
         *        selected.
         *      - Text value: that options display text.
         *
         * \param[in] optionMap Map of options and their values.
         */
        void setOptions(std::map<int, std::u32string> optionMap);
        //     void focus() override;
        //     void focusLost() override;
        /**
         * \brief Mouse click callback handler.
         *
         * Used to open the drop down menu popup.
         *
         * \sa InputDropDownWidget_Popup
         */
        void onClick(mouseEventInfo) override;
        /**
         * \brief Close the popup without changing the value.
         */
        void closePopup();
        /**
         * \brief Close the popup and set this widget's value to the new value.
         * \param[in] newValue  Value of the option that was picked.
         */
        void closePopup(int newValue);

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        int m_value;            ///< the current value of this input widget.
        std::map<int, std::u32string> m_options;    ///< The currently available options.
        TextWidget* m_pDisplay; ///< Reference to the text displaying the current value.
    };

}//name space
#endif
