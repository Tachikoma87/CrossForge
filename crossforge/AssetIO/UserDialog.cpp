#include <tinyfiledialogs/tinyfiledialogs.h>
#include "UserDialog.h"

using namespace std;

namespace CForge {
	string UserDialog::m_DefaultSavePath = "";
	string UserDialog::m_DefaultOpenPath = "";
	string UserDialog::m_DefaultFolderPath = "";

	void UserDialog::NotifyPopup(const std::string Title, const std::string Message, const IconType IType) {
		int32_t Rval = tinyfd_notifyPopup(Title.c_str(), Message.c_str(), getIconString(IType).c_str());
	}//NotifyPopup

	UserDialog::UserAnswer UserDialog::MessagePopup(const std::string Title, const std::string Message, const DialogType DType, const IconType IType, const bool DefaultYes) {
		int32_t DefaultButton = (DefaultYes) ? 1 : 0;

		int32_t Answer = tinyfd_messageBox(Title.c_str(), Message.c_str(), getDialogString(DType).c_str(), getIconString(IType).c_str(), DefaultButton);
		UserAnswer Rval = UserAnswer::ANSWER_UNKNOWN;
		switch (DType) {
		case DIALOG_OK: Rval = ANSWER_OK; break;
		case DIALOG_OKCANCEL: Rval = (Answer == 0) ? ANSWER_CANCEL : ANSWER_OK; break;
		case DIALOG_YESNO: Rval = (Answer == 0) ? ANSWER_NO : ANSWER_YES; break;
		default: Rval = ANSWER_UNKNOWN; break;
		}
		return Rval;
	}//MessageBox

	std::string UserDialog::InputBox(const std::string Title, const std::string Message) {
		const char* pStr = tinyfd_inputBox(Title.c_str(), Message.c_str(), "");
		return (nullptr == pStr) ? string("") : string(pStr);
	}//InputBox

	std::string UserDialog::PasswordBox(const std::string Title, const std::string Message) {
		const char* pStr = tinyfd_inputBox(Title.c_str(), Message.c_str(), nullptr);
		return (nullptr == pStr) ? string("") : string(pStr);
	}//PasswordBox

	std::string UserDialog::SaveFile(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription) {

		char** ppPatterns = buildFilterPatterns(FilterPatterns);
		const char* pStr = tinyfd_saveFileDialog(Title.c_str(), m_DefaultSavePath.c_str(), FilterPatterns.size(), ppPatterns, FilterDescription.c_str());
		deleteFilterPatterns(FilterPatterns, ppPatterns);
		return (nullptr == pStr) ? string("") : string(pStr);
	}//SaveFile

	std::string UserDialog::OpenFile(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription) {

		char** ppPatterns = buildFilterPatterns(FilterPatterns);
		const char* pStr = tinyfd_openFileDialog(Title.c_str(), m_DefaultOpenPath.c_str(), FilterPatterns.size(), ppPatterns, FilterDescription.c_str(), 0);
		deleteFilterPatterns(FilterPatterns, ppPatterns);

		return (nullptr == pStr) ? std::string("") : std::string(pStr);
	}//OpenFile

	std::vector<std::string> UserDialog::OpenFiles(const std::string Title, const std::vector<std::string> FilterPatterns, const std::string FilterDescription) {
		char** ppPatterns = buildFilterPatterns(FilterPatterns);
		const char* pStr = tinyfd_openFileDialog(Title.c_str(), m_DefaultOpenPath.c_str(), FilterPatterns.size(), ppPatterns, FilterDescription.c_str(), 1);
		deleteFilterPatterns(FilterPatterns, ppPatterns);

		// split string based on token
		vector<string> Rval;
		if (pStr != nullptr) {
			std::string s = "";
			for (auto i : string(pStr)) {
				if (i == '|') {
					Rval.push_back(s);
					s = "";
				}
				else {
					s += i;
				}
			}
			if (s.size() > 0) Rval.push_back(s);
		}
		return Rval;
	}//OpenFiles

	std::string UserDialog::SelectFolder(const std::string Title) {
		char* pStr = tinyfd_selectFolderDialog(Title.c_str(), m_DefaultFolderPath.c_str());
		return (nullptr == pStr) ? string("") : string(pStr);
	}//SelectFolder

	Eigen::Vector3i UserDialog::ColorPicker(const std::string Title, const Eigen::Vector3i DefaultRGB) {
		unsigned char DefRGB[3] = { uint8_t(DefaultRGB[0]), uint8_t(DefaultRGB[1]), uint8_t(DefaultRGB[2]) };
		unsigned char ResRGB[3] = { 0,0,0 };
		char* pStr = tinyfd_colorChooser(Title.c_str(), nullptr, DefRGB, ResRGB);
		return Eigen::Vector3i(ResRGB[0], ResRGB[1], ResRGB[2]);
	}//ColorPicker

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



	UserDialog::UserDialog() {

	}//UserDialog

	UserDialog::~UserDialog() {

	}//~UserDialog

	std::string UserDialog::getIconString(const IconType IType) {
		std::string Rval = "";
		switch (IType) {
		case ICON_INFO: Rval = "info"; break;
		case ICON_WARNING: Rval = "warning"; break;
		case ICON_ERROR: Rval = "error"; break;
		case ICON_QUESTION: Rval = "question"; break;
		default: Rval = "info"; break;
		}
		return Rval;
	}//getIconString

	std::string UserDialog::getDialogString(const DialogType DType) {
		std::string Rval = "";
		switch (DType) {
		case DIALOG_OK: Rval = "ok"; break;
		case DIALOG_OKCANCEL: Rval = "okcancel"; break;
		case DIALOG_YESNO: Rval = "yesno"; break;
		default: Rval = "okcancel"; break;
		}
		return Rval;
	}//getDialogString

	char** UserDialog::buildFilterPatterns(const std::vector<std::string> Patterns) {
		char** ppPatterns = nullptr;
		const uint32_t PatternCount = Patterns.size();

		if (Patterns.size() > 0) {
			ppPatterns = new char* [PatternCount];
			if (nullptr == ppPatterns) return nullptr;
			for (uint32_t i = 0; i < PatternCount; ++i) {
				ppPatterns[i] = new char[Patterns[i].size() + 1];
				ppPatterns[i][0] = '\0';
				memcpy(ppPatterns[i], Patterns[i].c_str(), Patterns[i].size());
				ppPatterns[i][Patterns[i].size()] = '\0';
			}
		}
		return ppPatterns;
	}//buildFilePatterns

	void UserDialog::deleteFilterPatterns(const std::vector<std::string> Patterns, char** ppPatterns) {
		for (uint32_t i = 0; i < Patterns.size(); ++i) {
			if (nullptr != ppPatterns && nullptr != ppPatterns[i]) delete[](ppPatterns[i]);
		}
		if (nullptr != ppPatterns) delete[] ppPatterns;
	}//deleteFilePatterns

}//name-space