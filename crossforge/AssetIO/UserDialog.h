/*****************************************************************************\
*                                                                           *
* File(s): UserDialog.h and UserDialog.cpp                                   *
*                                                                           *
* Content:  *
*                                                   *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_USERDIALOG_H__
#define __CFORGE_USERDIALOG_H__

#include "../Core/CForgeObject.h"

namespace CForge {

	class CFORGE_API UserDialog {
	public:
		enum IconType : int8_t {
			ICON_UNKNOWN = -1,
			ICON_INFO,
			ICON_WARNING,
			ICON_ERROR,
			ICON_QUESTION
		};

		enum DialogType : int8_t {
			DIALOG_UNKNOWN = -1,
			DIALOG_OK,
			DIALOG_OKCANCEL,
			DIALOG_YESNO
		};

		enum UserAnswer : int8_t {
			ANSWER_UNKNOWN = -1,
			ANSWER_OK,
			ANSWER_CANCEL,
			ANSWER_YES,
			ANSWER_NO
		};

		static void NotifyPopup(const std::string Title, const std::string Message, const IconType IType);
		static UserAnswer MessagePopup(const std::string Title, const std::string Message, const DialogType DType, const IconType IType, const bool DefaultYes);
		static std::string InputBox(const std::string Title, const std::string Message);
		static std::string PasswordBox(const std::string Title, const std::string Message);
		static std::string SaveFile(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription);
		static std::string OpenFile(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription);
		static std::vector<std::string> OpenFiles(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription);
		static std::string SelectFolder(const std::string Title);
		static Eigen::Vector3i ColorPicker(const std::string Title, const Eigen::Vector3i DefaultRGB);

		static void DefaultSavePath(const std::string DefaultPath);
		static void DefaultOpenPath(const std::string DefaultPath);
		static void DefaultFolderPath(const std::string DefaultPath);
		static std::string DefaultSavePath();
		static std::string DefaultOpenPath();
		static std::string DefaultFolderPath();

	protected:
		UserDialog();
		~UserDialog();

		static std::string getIconString(const IconType IType);
		static std::string getDialogString(const DialogType DType);

		static char** buildFilterPatterns(const std::vector<std::string> Patterns);
		static void deleteFilterPatterns(const std::vector<std::string> Patterns, char** ppPatterns);

		static std::string m_DefaultSavePath;
		static std::string m_DefaultOpenPath;
		static std::string m_DefaultFolderPath;

	};//UserDialog

}//name-space


#endif 