#include <portable-file-dialogs.h>
#include "UserDialog.h"

using namespace std;

namespace CForge {
	string UserDialog::m_DefaultSavePath = "";
	string UserDialog::m_DefaultOpenPath = "";
	string UserDialog::m_DefaultFolderPath = "";


	pfd::icon retrieveIconFromIconType(const UserDialog::IconType Icon) {
		pfd::icon Rval;
		switch (Icon) {
		case UserDialog::ICON_INFO: Rval = pfd::icon::info; break;
		case UserDialog::ICON_QUESTION: Rval = pfd::icon::question;  break;
		case UserDialog::ICON_WARNING: Rval = pfd::icon::warning; break;
		case UserDialog::ICON_ERROR: Rval = pfd::icon::error;  break;
		default: Rval = pfd::icon::info; break;
		}
		return Rval;
	}

	UserDialog::UserAnswer retrieveAnswerFromButton(const pfd::button Button) {
		UserDialog::UserAnswer Rval = UserDialog::ANSWER_UNKNOWN;;
		switch (Button) {
		case pfd::button::ok: Rval = UserDialog::ANSWER_OK; break;
		case pfd::button::cancel: Rval = UserDialog::ANSWER_CANCEL; break;
		case pfd::button::yes: Rval = UserDialog::ANSWER_YES; break;
		case pfd::button::no: Rval = UserDialog::ANSWER_NO; break;
		default: break;
		}
		return Rval;
	}

	pfd::choice retrieveChoiceFromDialogType(const UserDialog::DialogType Type) {
		pfd::choice Rval = pfd::choice::ok;
		switch (Type) {
		case UserDialog::DIALOG_OK: Rval = pfd::choice::ok; break;
		case UserDialog::DIALOG_OKCANCEL: Rval = pfd::choice::ok_cancel;  break;
		case UserDialog::DIALOG_YESNO: Rval = pfd::choice::yes_no; break;
		case UserDialog::DIALOG_YESNOCANCEL: Rval = pfd::choice::yes_no_cancel; break;
		default: break;
		}
		return Rval;
	}

	void UserDialog::NotifyPopup(const std::string Title, const std::string Message, const IconType IType) {
		pfd::notify(Title, Message, retrieveIconFromIconType(IType));
	}//NotifyPopup

	UserDialog::UserAnswer UserDialog::MessagePopup(const std::string Title, const std::string Message, const DialogType DType, const IconType IType) {
		pfd::button Answer = pfd::message(Title, Message, retrieveChoiceFromDialogType(DType), retrieveIconFromIconType(IType)).result();
		return retrieveAnswerFromButton(Answer);
	}//MessageBox


	std::string UserDialog::SaveFile(const std::string Title, const std::string FilterDescription, const std::string FilterPatterns) {
		std::vector<std::string> Filter = { FilterDescription + " (" + FilterPatterns + ")", FilterPatterns};
		return pfd::save_file::save_file(Title, m_DefaultSavePath, Filter).result();
	}//SaveFile

	std::string UserDialog::OpenFile(const std::string Title, const std::string FilterDescription, const std::string FilterPatterns) {
		std::vector<std::string> Filter = { FilterDescription + " (" + FilterPatterns + ")", FilterPatterns };
		std::vector<std::string> Result = pfd::open_file::open_file(Title, m_DefaultOpenPath, Filter, false).result();
		return (Result.size() > 0) ? Result[0] : "";
	}//OpenFile

	std::vector<std::string> UserDialog::OpenFiles(const std::string Title, const std::string FilterDescription, const std::string FilterPatterns) {
		std::vector<std::string> Filter = { FilterDescription + " (" + FilterPatterns + ")", FilterPatterns };
		return pfd::open_file::open_file(Title, m_DefaultOpenPath, Filter, true).result();
	}//OpenFiles

	std::string UserDialog::SelectFolder(const std::string Title) {
		if (!m_DefaultFolderPath.empty()) return pfd::select_folder::select_folder(Title, m_DefaultFolderPath, pfd::opt::force_path).result();
		return pfd::select_folder::select_folder(Title).result();
	}//SelectFolder

	void UserDialog::DefaultSavePath(const std::string DefaultPath) {
		m_DefaultSavePath = DefaultPath;
	}//DefaultSavePath

	void UserDialog::DefaultOpenPath(const std::string DefaultPath) {
		m_DefaultOpenPath = DefaultPath;
	}//DefaultStorePath

	void UserDialog::DefaultFolderPath(const std::string DefaultPath) {
		m_DefaultFolderPath = DefaultPath;
	}//DefaultFolderPath

	std::string UserDialog::DefaultSavePath() {
		return m_DefaultSavePath;
	}//DefaultSavePath

	std::string UserDialog::DefaultOpenPath() {
		return m_DefaultOpenPath;
	}//DefaultStorePath

	std::string UserDialog::DefaultFolderPath() {
		return m_DefaultFolderPath;
	}//DefaultFolderPath



	UserDialog::UserDialog(): CForgeObject("UserDialog") {

	}//UserDialog

	UserDialog::~UserDialog() {

	}//~UserDialog


}//name-space