/*****************************************************************************\
*                                                                           *
* File(s): InputDeviceController.h and InputDeviceController.cpp            *
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
#ifndef __CROSSFORGE_INPUTDEVICECONTROLLER_H__
#define __CROSSFORGE_INPUTDEVICECONTROLLER_H__

#include <crossforge/input/entities/InputDeviceEntity.h>
#include <crossforge/ecs/ControllerBase.h>
#include "../components/UserDialogComponent.h"
#include "../components/UserInputComponent.h"

namespace crossforge {
	class InputDeviceController : public ControllerBase {
	public:
		static inline std::string identification = "InputDeviceController";
		
		static bool conductUserDialog(InputDeviceEntityPtr pInputDevice);

		
		static bool conjureNotifyPopup(std::string title, std::string message, UserDialogComponent::IconType icon);
		static bool conjureMessageBox(std::string title, std::string message, UserDialogComponent::MessageBoxType msgBoxType, UserDialogComponent::IconType iconType, UserInputComponentPtr pResult);
		static bool conjureInputBox(std::string title, std::string message, std::string& userInput, std::string defaultInput, bool isPassword);
		static bool conjureSaveFileDialog(std::string title, std::string defaultPathOrFile, std::vector<std::string> filterPatterns, std::string& saveFile);
		static bool conjureOpenFileDialog(std::string title, std::string defaultPath, std::vector<std::string> filterPatterns, bool allowMultiselect, std::vector<std::string>& fileList);
		static bool conjureSelectDirectoryDialog(std::string title, std::string defaultPath, std::string& directoryPath);
		static bool conjurColorPicker(std::string title, Eigen::Vector3f& rgbColor);

	protected:
		InputDeviceController(const std::string childIdentification);
		~InputDeviceController();

		static std::string getIconNameFromType(UserDialogComponent::IconType icon);
		static std::string getMessageBoxNameFromType(UserDialogComponent::MessageBoxType type);
	};

	typedef std::shared_ptr<InputDeviceController> InputDeviceControllerPtr;
}

#endif 