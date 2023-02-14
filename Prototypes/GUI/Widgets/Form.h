/*****************************************************************************\
*                                                                           *
* File(s): Form.h and Form.cpp                                      *
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
#ifndef __CFORGE_FORM_H__
#define __CFORGE_FORM_H__

#include "../Widget.h"
#include "../GUI.h"

#include "Label.h"
#include <CForge/Core/ITCaller.hpp>

namespace CForge {

    class FormWidget;

    /**
     * \brief Button to explicitly trigger the FormWidget's program callback.
     * \ingroup GUI
     */
    class FormWidget_ApplyButton : public TextWidget {
    public:
        FormWidget_ApplyButton(GUI* rootGUIObject, FormWidget* parent);

        /**
         * \brief Mouse click callback handler.
         *
         * Used to trigger the FormWidget's program callback.
         *
         * \param[in] mouse     Mouse cursor position.
         * \sa CForge::FormWidget CForge::FormWidget::sendCallback
         */
        void onClick(mouseEventInfo) override;
    private:
        FormWidget* m_parentForm;   ///< Reference to the parent FormWidget.
    };

    /**
     * \brief A widget representing labeled inputs, able to create callbacks.
     *
     * As outlined in CForge::GUI::createOptionsWindow, an empty applyName will
     * lead to no apply button being used and any value change triggering the
     * callback. Otherwise, the button will be named as specified and callbacks
     * will only trigger upon button presses.
     *
     * To receive the callback, interested objects need to implement the
     * CForge::ITListener<GUICallbackObject> interface and then register for
     * callbacks from this widget by calling its inherited
     * CForge::ITCaller::startListening method.
     *
     * \ingroup GUI
     */
    class FormWidget : public BaseWidget, public CForge::ITCaller<GUICallbackObject> {
    public:
        /**
         * \brief Initialises the widget.
         *
         * If the applyName parameter is not an empty string, this will also create
         * the apply button widget.
         *
         * \param[in] FormID    The ID used to identify the form in the program's callback code.
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         * \param[in] applyName The text the apply button should display.
         */
        FormWidget(int FormID, GUI* rootGUIObject, BaseWidget* parent, std::u32string applyName = U"Apply");
        ~FormWidget();

        /**
         * \brief Add a new option to the form.
         *
         * \param[in] OptionID  The ID used to identify the option in the program's callback code.
         * \param[in] type      What input widget should be used for the option.
         * \param[in] name      What (descriptive) text should be displayed alongside the input. Can be multiline if necessary.
         */
        void addOption(int OptionID, GUIInputType type, std::u32string name);
        //     template <typename T>
        //     auto addOption(int OptionID, GUIInputType type, std::u32string name);

        /**@{*/
        /**
         * \brief Set limits to the input value range.
         *
         * These methods work on many input widgets and as such it's best to check
         * the specific widget's documentation for more details on what exactly they
         * do.
         *
         * In general, on widgets dealing with number input they will limit the value
         * range, and on the default text input the amount of characters is checked.
         *
         * The integer functions should work on all supported widgets, however the
         * float variant will only work on float input widgets. If no lower bound is
         * specified, it will default to 0.
         *
         * \param[in] OptionID  ID of the option which value range shall be limited.
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(int OptionID, int higher);
        void setLimit(int OptionID, int lower, int higher);
        void setLimit(int OptionID, float higher);
        void setLimit(int OptionID, float lower, float higher);
        /**@}*/
        /**@{*/
        /**
         * \brief Set the current value for the specified input.
         *
         * Methods for the input widgets' various data types to set the current value.
         * While this is usually used to set the default value during set up (hence the
         * name), it could also be used during runtime if desired.
         *
         * Changing the values through these methods will not trigger a callback.
         *
         * \param[in] OptionID  ID of the option which value shall be set.
         * \param[in] value     The value that should be applied.
         */
        void setDefault(int OptionID, int value);
        void setDefault(int OptionID, float value);
        void setDefault(int OptionID, bool value);
        void setDefault(int OptionID, std::u32string value);
        /**@}*/
        /**
         * \brief Set the step size for a slider input.
         * \param[in] OptionID  ID of the option, should be a slider input.
         * \param[in] stepSize  What value delta one step should be.
         */
        void setStepSize(int OptionID, float stepSize);
        /**
         * \brief Set the options of a drop down input.
         *
         * The optionMap should be structured as follows:
         *      - the integer key is the expected return value when that option
         *        is currently selected.
         *      - the std::u32string is the accompanying text that should be
         *        displayed in the drop down menu.
         *
         * \param[in] OptionID  ID of the option, should be a drop down input.
         * \param[in] optionMap The options it should have.
         */
        void setDropDownOptions(int OptionID, std::map<int, std::u32string> optionMap);

        /**
         * \brief Triggers the program callback.
         * \sa CForge::GUICallbackObject
         */
        void sendCallback();

        /** Used for the immediate callback mode.
         *  \param[in] child    Reference to the widget that had its value changed. */
        void childValueChanged(BaseWidget* child) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;

        //for testing
        /** Unused. */
        void onDrag(mouseEventInfo mouse) override;
    private:
        int m_FormID;                                   ///< The form's ID to identify it in callbacks.
        std::unordered_map<int, LabelWidget*> m_labels; ///< The form's current options. Integer key is the OptionID.
        FormWidget_ApplyButton* m_ApplyButton;          ///< Reference to the apply button (if applicable).
        bool dontSendCallback = false;                  ///< Used for immediate callback mode to prevent callbacks when setting values through the form methods.
    };

}//name space
#endif
