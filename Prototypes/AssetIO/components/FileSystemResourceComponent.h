/*****************************************************************************\
*                                                                           *
* File(s): FileSystemResourceComponent.h and FileSystemResourceComponent.cpp            *
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
#ifndef __CROSSFORGE_FILESYSTEMRESOURCECOMPONENT_H__
#define __CROSSFORGE_FILESYSTEMRESOURCECOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class FileSystemResourceComponent : public ComponentBase {
	public:
		inline static std::string identification = "FileSystemResourceComponent";

		enum AccessMode : int8_t {
			MODE_UNKNOWN = -1,
			MODE_READ = 0,
			MODE_WRITE,
			MODE_APPEND,
			MODE_READ_EXTENDED,
			MODE_WRITE_EXTENED,
			MODE_APPEND_EXTENDED
		};

		FileSystemResourceComponent();
		~FileSystemResourceComponent();

		void initialize();
		void clear();
		
		std::unique_ptr<FILE*> &handle();
		bool& binaryMode();
		AccessMode& accessMode();
		std::string &originalFilepath();

	protected:
		std::unique_ptr<FILE*> m_pHandle;
		AccessMode m_accessMode;
		bool m_binaryMode;
		std::string m_originalFilepath;
	};
	typedef std::shared_ptr<FileSystemResourceComponent> FileSystemResourceComponentPtr;
}

#endif