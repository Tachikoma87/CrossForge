/*****************************************************************************\
*                                                                           *
* File(s): FilePathsComponent.h and FilePathsComponent.cpp                  *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_USERDIALOGCOMPONENT_H__
#define __CROSSFORGE_USERDIALOGCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class UserDialogComponent : public ComponentBase {
	public:
		static inline std::string identification = "UserDialogComponent";

		enum DialogType: int8_t {
			DIALOG_TYPE_UNKNOWN = -1,
			DIALOG_TYPE_POPUP = 0,
			DIALOG_TYPE_MESSAGE_BOX,
			DIALOG_TYPE_INPUT_BOX,
			DIALOG_TYPE_INPUT_PASSWORD,
			DIALOG_TYPE_SAVE_FILE,
			DIALOG_TYPE_OPEN_FILE,
			DIALOG_TYPE_SELECT_DIRECTORY,
			DIALOG_TYPE_COLOR_PICKER,
			DIALOG_TYPE_COUNT,
		};

		enum IconType:int8_t {
			ICON_TYPE_UNKNOWN = -1,
			ICON_TYPE_INFO = 0,
			ICON_TYPE_WARNING,
			ICON_TYPE_ERROR,
			ICON_TYPE_QUESTION,
			ICON_TYPE_COUNT
		};

		enum MessageBoxType:int8_t {
			MESSAGE_BOX_TYPE_UNKNOWN = -1,
			MESSAGE_BOX_TYPE_OK = 0,
			MESSAGE_BOX_TYPE_OK_CANCEL,
			MESSAGE_BOX_TYPE_YES_NO,
			MESSAGE_BOX_TYPE_YES_NO_CANCEL,
			MESSAGE_BOX_TYPE_COUNT
		};

		UserDialogComponent();
		~UserDialogComponent();

		void initialize();
		void clear() override;

		std::string& title();
		std::string& message();
		std::string& defaultInput();
		std::string& defaultPath();
		std::vector<std::string>& fileFilterPatterns();
		bool& allowMultiselect();

		DialogType& dialogType();
		IconType& iconType();
		MessageBoxType& messageBoxType();

	protected:
		UserDialogComponent(const std::string childIdentification);

		DialogType m_dialogType;
		IconType m_iconType;
		MessageBoxType m_messageBoxType;
		std::string m_title;
		std::string m_message;
		std::string m_defaultInput;
		bool m_allowMultiselect;

		std::vector<std::string> m_fileFilterPatterns;
		std::string m_defaultPath;
	};

	typedef std::shared_ptr<UserDialogComponent> UserDialogComponentPtr;
}

#endif