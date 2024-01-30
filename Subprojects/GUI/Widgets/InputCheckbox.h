/*****************************************************************************\
*                                                                           *
* File(s): InputCheckbox.h and InputCheckbox.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): SimonKretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_INPUTCHECKBOX_H__
#define __CFORGE_INPUTCHECKBOX_H__

#include "../Widget.h"
#include "../GUI.h"

namespace CForge {

    /**
     * \brief A simple checkbox input widget.
     *
     * Used to set a boolean value.
     *
     * \ingroup GUI
     */
    class InputCheckboxWidget : public BaseWidget {
    public:
        InputCheckboxWidget(GUI* rootGUIObject, BaseWidget* parent);
        ~InputCheckboxWidget();

        bool getValue();                        ///< Returns whether the checkbox is currently checked.

        /**
         * \brief Sets the checkbox state.
         *
         * If true is passed in, the checkmark will be shown.
         * If false, the checkbox will be empty/unchecked.
         *
         * \param[in] state     The state the checkbox should be set to.
         */
        void setState(bool state);

        void onClick(mouseEventInfo) override;  ///< Toggles whether the checkbox is checked.

        void changePosition(float dx, float dy) override;
        /*void updateLayout() override;*/   //this widget does not change its size
        void draw(CForge::RenderDevice* renderDevice) override;
    private:
        bool m_state;               ///< Whether the checkbox is currently checked.
        TextWidget* m_checkmark;    ///< Reference to the TextWidget used for the checkmark.
    };

}//name space
#endif
