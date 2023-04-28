/*****************************************************************************\
*                                                                           *
* File(s): Label.h and Label.cpp                                      *
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
#ifndef __CFORGE_LABEL_H__
#define __CFORGE_LABEL_H__

#include "../Widget.h"
#include "../GUI.h"

#include "InputNumber.h"
#include "InputCheckbox.h"
#include "InputText.h"
#include "InputSlider.h"
#include "InputDropDown.h"

namespace CForge {

    /**
     * \brief Widget to abstract different value types of the input widgets.
     *
     * Pairs an input widget with a (potentially multiline) text label in
     * an agnostic API.
     *
     * \ingroup GUI
     */
    class LabelWidget : public BaseWidget {
    public:
        /**
         * \brief Initialises the widget.
         *
         * Creates the input widget, the CForge::TextWidget for the label text
         * and memory to buffer the widget's return value. As usual, cleanup is
         * handled entirely by this widget's destructor.
         *
         * \param[in] type      What input widget should be wrapped.
         * \param[in] labelText The text it should be labeled with.
         * \param[in] rootGUIObject Reference to the main GUI class.
         * \param[in] parent    The widget's parent widget.
         */
        LabelWidget(GUIInputType type, std::u32string labelText, GUI* rootGUIObject, BaseWidget* parent);
        ~LabelWidget();

        /**
         * \brief Fetch the value from the input, buffer it and return a CForge::GUICallbackDatum.
         */
        GUICallbackDatum getValue();
        /**@{*/
        /**
         * \brief Set limits to the input value range.
         *
         * Limit the valid value range of the wrapped input widget. Check the
         * input's documentation for more details on how exactly the limits apply.
         *
         * \param[in] lower     Lower limit.
         * \param[in] higher    Upper limit.
         */
        void setLimit(int lower, int higher);
        void setLimit(float lower, float higher);
        /**@}*/
        /**@{*/
        /**
         * \brief Set the current value for the specified input.
         * \param[in] value     The value that should be applied.
         */
        void setDefault(int value);
        void setDefault(float value);
        void setDefault(bool value);
        void setDefault(std::u32string value);
        /**@}*/
        /**
         * \brief Set the step size for a slider input.
         * \param[in] stepSize  What value delta one step should be.
         */
        void setStepSize(float stepSize);

        /**
         * \brief Set the options of a drop down input.
         *
         * The optionMap should be structured as follows:
         *      - the integer key is the expected return value when that option
         *        is currently selected.
         *      - the std::u32string is the accompanying text that should be
         *        displayed in the drop down menu.
         *
         * \param[in] optionMap The options it should have.
         */
        void setOptions(std::map<int, std::u32string> optionMap);

        //     template <typename T> auto getInputWidget();

        /**
         * \brief Get the width of only the text label and any spacing in front of
         *        the input widget.
         */
        float getJustification();
        /**
         * \brief Positions the input widget to match the specified justification spacing value.
         *
         * The justification value specifies how far from the LabelWidget's origin
         * the input widget will be positioned. This is useful to align multiple
         * options of a form to a visually uniform list by setting the justification
         * of all LabelWidgets to that of the option with the longest text.
         *
         * \param[in] j     The justification value that should be aligned to.
         */
        void setJustification(float j);

        /** Passes through to the parent widget (usually CForge::FormWidget),
         *  that an input was interacted with.
         *  \param[in] child Reference to the widget that had its value changed. */
        void childValueChanged(BaseWidget* child) override;

        void changePosition(float dx, float dy) override;
        void updateLayout() override;
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        GUIInputType m_type;        ///< What input type this label currently wraps.
        void* m_pValue;             /**< A buffered copy of the input's return value.
                                         The reason for the buffering is that the callback code
                                         needs to perform some potentially confusing pointer casting.
                                         To prevent accidental writes to the value from potentially
                                         destroying an input widget's state, the value is buffered instead.*/
        TextWidget* m_pLabelText;   ///< Reference to the label text's widget.
        BaseWidget* m_pInput;       ///< Reference to the input widget.
        float m_justification;      ///< offset of the input element. \sa getJustification setJustification
    };

}//name space
#endif
