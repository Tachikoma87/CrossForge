/*****************************************************************************\
*                                                                           *
* File(s): UserDialog.h and UserDialog.cpp                                  *
*                                                                           *
* Content: Class to perform user dialogs such as message boxes, file        *
*          dialogs, and notifications.                                      *
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
	/**
	* \brief A class with mostly static methods that performs user dialogs such as message boxes, file dialogs, and notifications.
	* \ingroup AssetIO
	* 
	*/
	class CFORGE_API UserDialog: public CForgeObject {
	public:
		/**
		* \brief Enumerate of available icons.
		*/
		enum IconType : int8_t {
			ICON_UNKNOWN = -1,	///< Default value.
			ICON_INFO,			///< Info icon.
			ICON_WARNING,		///< Warning icon.
			ICON_ERROR,			///< Error icon.
			ICON_QUESTION		///< Question mark icon.
		};

		/**
		* \brief Enumerate of available dialog types.
		*/
		enum DialogType : int8_t {
			DIALOG_UNKNOWN = -1,	///< Default value.
			DIALOG_OK,				///< Dialog with ok button.
			DIALOG_OKCANCEL,		///< Dialog with ok and cancel buttons.
			DIALOG_YESNO,			///< Dialog with yes and no buttons.
			DIALOG_YESNOCANCEL		///< Dialog with yes, no, and cancel buttons.
		};

		/**
		* \brief Enumerate of button pressed by the user.
		*/
		enum UserAnswer : int8_t {
			ANSWER_UNKNOWN = -1,	///< Default value.
			ANSWER_OK,			///< Ok button pressed.
			ANSWER_CANCEL,		///< Cancel button pressed.
			ANSWER_YES,			///< Yes button pressed.
			ANSWER_NO			///< No button pressed.
		};

		/**
		* \brief Shows a notification in the system's tray.
		* 
		* \param[in] Title The title of the notification.
		* \param[in] Message The message shown.
		* \param[in] IType Icon that should be shown.
		*/
		static void NotifyPopup(const std::string Title, const std::string Message, const IconType IType = ICON_INFO);

		/**
		* \brief A popup window showing a message.
		* 
		* \param[in] Title Caption shown in the title bar of the window.
		* \param[in] Message Content text of the window.
		* \param[in] DType Dialog type, which determines the available buttons.
		* \param[in] IType Which icon is shown in the popup message.
		* \return Which button was clicked by the user.
		*/
		static UserAnswer MessagePopup(const std::string Title, const std::string Message, const DialogType DType, const IconType IType);

		/**
		* \brief Save file dialog to get a file path to store a file.
		* 
		* \param[in] Title Caption shown in the title bar of the window.
		* \param[in] FilterDescription Label of the file filter. The patterns string will be attached.
		* \param[in] FilterPatterns Filter patters as a single string separated whit a whitespace, e.g. "*.jpg *.webp *.png".
		* \return Path to the selected file or empty string if selection was aborted. 
		*/
		static std::string SaveFile(const std::string Title, const std::string FilterDescription = "All Files", const std::string FilterPatterns = "*");

		/**
		* \brief Open file dialog to retrieve a file path.
		* 
		* \param[in] Title Caption shown in the title bar of the window.
		* \param[in] FilterDescription Label of the file filter. The patterns string will be attached.
		* \param[in] FilterPatterns Filter patterns as a single string separated with a whitespace, e.g. "*.jpg *.webp *.png".
		* \return Path to the selected file or empty string if selection was aborted.
		*/
		static std::string OpenFile(const std::string Title, const std::string FilterDescription = "All Files", const std::string FilterPatterns = "*");

		/**
		* \brief Open file dialog to retrieve a set of file paths.
		* 
		* \param[in] Title Caption shown in the title bar of the window.
		* \param[in] FilterDescription Label of the file filter.
		* \param[in] FilterPatterns Filter patterns as a single string separated with a whitespace, e.g. "*.jpg *.web *.png"
		* \return List of selected files or empty list if selection was aborted.
		*/
		static std::vector<std::string> OpenFiles(const std::string Title, const std::string FilterDescription = "All Files", const std::string FilterPatterns = "*");

		/**
		* \brief Select folder dialog to retrieve a path to a folder.
		* 
		* \param[in] Title Caption shown in the title bar of the window.
		* \return Path to the selected folder or empty string if selection was aborted.
		*/
		static std::string SelectFolder(const std::string Title);

		/**
		* \brief Set the initial path for the save file dialog.
		* 
		* \param[in] DefaultPath The initial path.
		*/
		static void DefaultSavePath(const std::string DefaultPath);

		/**
		* \brief Set the initial path for the open file and open files dialog.
		* 
		* \param[in] DefaultPath The initial path.
		*/
		static void DefaultOpenPath(const std::string DefaultPath);

		/**
		* \brief Set the initial path for the select folder dialog.
		* 
		* \param[in9 DefaultPath The initial path.
		*/
		static void DefaultFolderPath(const std::string DefaultPath);

		/**
		* \brief Getter for the initial path of the save file dialog.
		* 
		* \return Initial path of the save file dialog.
		*/
		static std::string DefaultSavePath();

		/**
		* \brief Getter for the initial path of the open file and open files dialog.
		* 
		* \return Initial path of the open file and open files dialog.
		*/
		static std::string DefaultOpenPath();


		/**
		* \brief Getter for the initial path of the select folder path.
		* 
		* \return Initial path of the select folder dialog.
		*/
		static std::string DefaultFolderPath();

	protected:
		/**
		* \brief Constructor
		*/
		UserDialog();

		/**
		* \brief Destructor
		*/
		~UserDialog();

		static std::string m_DefaultSavePath;	///< Default save file dialog path.
		static std::string m_DefaultOpenPath;	///< Default open file dialog path.
		static std::string m_DefaultFolderPath; ///< Default open folder dialog path.

	};//UserDialog

}//name-space


#endif 