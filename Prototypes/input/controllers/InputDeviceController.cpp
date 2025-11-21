#include "InputDeviceController.h"
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "../components/UserInputComponent.h"
#include "../../utility/MiscUtility.hpp"

namespace crossforge {

	InputDeviceController::InputDeviceController(const std::string childIdentification): ControllerBase(InputDeviceController::identification) {

	}
	InputDeviceController::~InputDeviceController() {

	}

	bool InputDeviceController::conductUserDialog(InputDeviceEntityPtr pInputDevice) {
		if (nullptr == pInputDevice) throw NullpointerExcept("pInputDevice");
		auto pUserDialogComp = pInputDevice->getComponent<UserDialogComponent>();
		if (nullptr == pUserDialogComp) throw MissingComponentException(UserDialogComponent::identification);

		bool result = false;
		switch (pUserDialogComp->dialogType()) {
		case UserDialogComponent::DIALOG_TYPE_POPUP: {
			conjureNotifyPopup(pUserDialogComp->title(), pUserDialogComp->message(), pUserDialogComp->iconType());
		}break;
		case UserDialogComponent::DIALOG_TYPE_MESSAGE_BOX: {
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			result = conjureMessageBox(pUserDialogComp->title(), pUserDialogComp->message(), pUserDialogComp->messageBoxType(), pUserDialogComp->iconType(), pInputDevice->getComponent<UserInputComponent>());
		}break;
		case UserDialogComponent::DIALOG_TYPE_INPUT_BOX: {
			std::string userInput = "";
			result = conjureInputBox(pUserDialogComp->title(), pUserDialogComp->message(), userInput, pUserDialogComp->defaultInput(), false);
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			pUserInput->string() = userInput;
		}break;
		case UserDialogComponent::DIALOG_TYPE_INPUT_PASSWORD: {
			std::string userInput = "";
			result = conjureInputBox(pUserDialogComp->title(), pUserDialogComp->message(), userInput, pUserDialogComp->defaultInput(), true);
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			pUserInput->string() = userInput;
		}break;
		case UserDialogComponent::DIALOG_TYPE_SAVE_FILE: {
			std::string saveFile = "";
			result = conjureSaveFileDialog(pUserDialogComp->title(), pUserDialogComp->defaultPath(), pUserDialogComp->fileFilterPatterns(), saveFile);
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			pUserInput->fileList().clear();
			if(!saveFile.empty())pUserInput->fileList().push_back(saveFile);
		}break;
		case UserDialogComponent::DIALOG_TYPE_OPEN_FILE: {
			std::vector<std::string> fileList;
			result = conjureOpenFileDialog(pUserDialogComp->title(), pUserDialogComp->defaultPath(), pUserDialogComp->fileFilterPatterns(), pUserDialogComp->allowMultiselect(), fileList);
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			pUserInput->fileList() = fileList;
		}break;
		case UserDialogComponent::DIALOG_TYPE_SELECT_DIRECTORY: {
			std::string directoryPath = "";
			result = conjureSelectDirectoryDialog(pUserDialogComp->title(), pUserDialogComp->defaultPath(), directoryPath);
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			pUserInput->directory() = directoryPath;
		}break;
		case UserDialogComponent::DIALOG_TYPE_COLOR_PICKER: {
			if (!pInputDevice->hasComponent(UserInputComponent::identification)) pInputDevice->addComponent(std::make_shared<UserInputComponent>());
			auto pUserInput = pInputDevice->getComponent<UserInputComponent>();
			result = conjurColorPicker(pUserDialogComp->title(), pUserInput->rgbColor());
		}break;
		default: {
			LogError("Not handled dialog type " + std::to_string(pUserDialogComp->dialogType()) + " encountered.");
		}break;
		}

		return result;
	}


	bool InputDeviceController::conjureNotifyPopup(std::string title, std::string message, UserDialogComponent::IconType icon) {
		if (title.empty()) title = "CrossForge Notification Popup";
		if (message.empty()) message = "The programmer forgot to provide a message :-(";
		if (UserDialogComponent::ICON_TYPE_UNKNOWN >= icon || UserDialogComponent::ICON_TYPE_COUNT >= icon) icon = UserDialogComponent::ICON_TYPE_INFO;
		return (tinyfd_notifyPopup(title.c_str(), message.c_str(), getIconNameFromType(icon).c_str()) > 0);
	}
	bool InputDeviceController::conjureMessageBox(std::string title, std::string message, UserDialogComponent::MessageBoxType msgBoxType, UserDialogComponent::IconType iconType, UserInputComponentPtr pResult) {
		if (nullptr == pResult) throw NullpointerExcept("pResult");
		if (title.empty()) title = "CrossForge Message Box";
		if (message.empty()) message = "The programmer forgot to provide a message :-(";
		if (UserDialogComponent::MESSAGE_BOX_TYPE_UNKNOWN >= msgBoxType || UserDialogComponent::MESSAGE_BOX_TYPE_COUNT <= msgBoxType) throw IndexOutOfBoundsExcept("msgBoxType");
		if (UserDialogComponent::ICON_TYPE_UNKNOWN >= iconType || UserDialogComponent::ICON_TYPE_COUNT <= iconType) throw IndexOutOfBoundsExcept("msgBoxType");
		int32_t result = tinyfd_messageBox(title.c_str(), message.c_str(), getMessageBoxNameFromType(msgBoxType).c_str(), getIconNameFromType(iconType).c_str(), 0);
		// evaluate result
		switch (msgBoxType) {
		case UserDialogComponent::MESSAGE_BOX_TYPE_OK: {
			pResult->userAnswer() = UserInputComponent::USER_ANSWER_OK;
		}break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_YES_NO: {
			pResult->userAnswer() = (1 == result) ? UserInputComponent::USER_ANSWER_YES : UserInputComponent::USER_ANSWER_NO;
		}break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_OK_CANCEL: {
			pResult->userAnswer() = (1 == result) ? UserInputComponent::USER_ANSWER_OK : UserInputComponent::USER_ANSWER_CANCEL;
		}break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_YES_NO_CANCEL: {
			if (0 == result) pResult->userAnswer() = UserInputComponent::USER_ANSWER_CANCEL;
			else if (1 == result) pResult->userAnswer() = UserInputComponent::USER_ANSWER_YES;
			else pResult->userAnswer() = UserInputComponent::USER_ANSWER_NO;
		}break;
		default: {
			LogError("Not handled message box type " + std::to_string(msgBoxType));
			pResult->userAnswer() = UserInputComponent::USER_ANSWER_NO;
		}break;
		}
		return true;
	}
	bool InputDeviceController::conjureInputBox(std::string title, std::string message, std::string& userInput, std::string defaultInput, bool isPassword) {
		if (title.empty()) title = "CrossForge Input Box";
		if (message.empty()) message = "The programmer forgot to provide a message :-(";
		char const* pUserInput = tinyfd_inputBox(title.c_str(), message.c_str(), (isPassword) ? nullptr : defaultInput.c_str());
		if (nullptr != pUserInput) userInput = pUserInput;
		return (nullptr != pUserInput);
	}

	bool InputDeviceController::conjureSaveFileDialog(std::string title, std::string defaultPathOrFile, std::vector<std::string> filterPatterns, std::string &saveFile) {
		std::vector<const char*> patterns;
		for (std::string &p : filterPatterns) patterns.push_back(p.c_str());
		char const* pSaveFile = tinyfd_saveFileDialog(title.c_str(), (defaultPathOrFile.empty()) ? nullptr : defaultPathOrFile.c_str(), patterns.size(), patterns.data(), nullptr);
		if (nullptr != pSaveFile) saveFile = pSaveFile;
		return (nullptr != pSaveFile);
	}
	bool InputDeviceController::conjureOpenFileDialog(std::string title, std::string defaultPath, std::vector<std::string> filterPatterns, bool allowMultiselect, std::vector<std::string>& fileList) {
		std::vector<const char*> patterns;
		for (std::string& p : filterPatterns) patterns.push_back(p.c_str());

		char const * pResult = tinyfd_openFileDialog(title.c_str(), (defaultPath.empty()) ? nullptr : defaultPath.c_str(), patterns.size(), patterns.data(), nullptr, allowMultiselect);
		// if multiple files are selected then delimiter is |
		fileList.clear();
		if (nullptr != pResult) {
			fileList = GeneralUtility::splitString(pResult, '|');
		}
		return (nullptr != pResult);
	}

	bool InputDeviceController::conjureSelectDirectoryDialog(std::string title, std::string defaultPath, std::string& directoryPath) {
		char const* pDirectoryPath = tinyfd_selectFolderDialog(title.c_str(), (defaultPath.empty()) ? nullptr : defaultPath.c_str());
		if (nullptr != pDirectoryPath) directoryPath = pDirectoryPath;
		return (nullptr != pDirectoryPath);
	}

	bool InputDeviceController::conjurColorPicker(std::string title, Eigen::Vector3f& rgbColor) {
		if (title.empty()) title = "CrossForge Color Picker";
		uint8_t rgb[3] = { uint8_t(rgbColor.x()*255), uint8_t(rgbColor.y()*255), uint8_t(rgbColor.z()*255) };
		bool result = false;
		if (nullptr != tinyfd_colorChooser(title.c_str(), nullptr, rgb, rgb)) {
			rgbColor.x() = rgb[0]/255.0f;
			rgbColor.y() = rgb[1]/255.0f;
			rgbColor.z() = rgb[2] / 255.0f;
			result = true;
		}
		return result;
	}

	std::string InputDeviceController::getIconNameFromType(UserDialogComponent::IconType icon) {
		if (UserDialogComponent::ICON_TYPE_UNKNOWN >= icon || UserDialogComponent::ICON_TYPE_COUNT <= icon) throw IndexOutOfBoundsExcept("icon");
		std::string result = "";
		switch (icon) {
		case UserDialogComponent::ICON_TYPE_INFO: result = "info"; break;
		case UserDialogComponent::ICON_TYPE_WARNING: result = "warning"; break;
		case UserDialogComponent::ICON_TYPE_ERROR: result = "error"; break;
		case UserDialogComponent::ICON_TYPE_QUESTION: result = "question"; break;
		default: {
			LogError("Not handled icon type " + std::to_string(icon) + " encountered.");
		}break;
		}
		return result;
	}

	std::string InputDeviceController::getMessageBoxNameFromType(UserDialogComponent::MessageBoxType type) {
		if (UserDialogComponent::MESSAGE_BOX_TYPE_UNKNOWN >= type || UserDialogComponent::MESSAGE_BOX_TYPE_COUNT <= type) throw IndexOutOfBoundsExcept("type");
		std::string result = "";
		switch (type) {
		case UserDialogComponent::MESSAGE_BOX_TYPE_OK: result = "ok"; break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_OK_CANCEL: result = "okcancel"; break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_YES_NO: result = "yesno"; break;
		case UserDialogComponent::MESSAGE_BOX_TYPE_YES_NO_CANCEL: result = "yesnocancel"; break;
		default: {
			LogError("Not handled message box type " + std::to_string(type) + " encountered.");
		}break;
		}
		return result;
	}
}