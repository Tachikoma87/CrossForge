/*****************************************************************************\
*                                                                           *
* File(s): UserInputComponent.h and UserInputComponent.cpp                  *
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
#ifndef __CROSSFORGE_USERINPUTCOMPONENT_H__
#define __CROSSFORGE_USERINPUTCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class UserInputComponent : public ComponentBase {
	public:
		static inline std::string identification = "UserInputComponent";
		enum UserAnswer : int8_t {
			USER_ANSWER_UNKNOWN = -1,
			USER_ANSWER_OK,
			USER_ANSWER_YES,
			USER_ANSWER_NO,
			USER_ANSWER_CANCEL,
			USER_ANSWER_COUNT,
		};

		UserInputComponent();
		~UserInputComponent();

		void initialize();
		void clear();

		Eigen::Vector3f& rgbColor();
		std::string& string();
		UserAnswer& userAnswer();
		std::vector<std::string>& fileList();
		std::string& directory();

	protected:
		UserInputComponent(const std::string childIdentification);

		Eigen::Vector3f m_rgbColor;
		std::string m_string;
		UserAnswer m_userAnswer;
		std::vector<std::string> m_fileList;
		std::string m_directory;
	};

	typedef std::shared_ptr<UserInputComponent> UserInputComponentPtr;
}

#endif 