/*****************************************************************************\
*                                                                           *
* File(s): FileEntity.h and FileEntity.cpp                              *
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
#ifndef __CROSSFORGE_FILEENTITY_H__
#define __CROSSFORGE_FILEENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/BinaryDataComponent.h"
#include "../components/FileSystemResourceComponent.h"
#include "../components/StringDataComponent.h"

namespace crossforge {
	class FileEntity : public EntityBase {
	public:
		inline static std::string identification = "FileEntity";

		enum FileCompnents: uint8_t {
			BINARY_DATA_COMPONENT = 0x01,
			FILE_SYSTEM_RESOURCE_COMPONENT = 0x02,
			STRING_DATA_COMPONENT = 0x04,
			ALL_COMPONENTS = 0xFF
		};

		FileEntity(uint8_t componentBitmask = 0);
		~FileEntity();

		void initialize(uint8_t componentBitmask);
		void clear();

		BinaryDataComponentPtr getBinaryDataComponent();
		FileSystemResourceComponentPtr getFileSystemResourceComponent();
		StringDataComponentPtr getStringDataComponent();

	protected:

	};
	typedef std::shared_ptr<FileEntity> FileEntityPtr;
}


#endif 