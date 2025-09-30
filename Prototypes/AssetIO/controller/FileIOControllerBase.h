/*****************************************************************************\
*                                                                           *
* File(s): FileIOControllerBase.h and FileIOControllerBase.cpp                  *
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
#ifndef __CROSSFORGE_FILEIOCONTROLLERBASE_H__
#define __CROSSFORGE_FILEIOCONTROLLERBASE_H__

#include "../../ECS/ControllerBase.h"
#include "../entities/FileEntity.h"

namespace crossforge {
	class FileIOControllerBase: public ControllerBase {
	public:

		virtual bool load(FileEntityPtr pEntity, const std::string filepath, bool binary) = 0;
		virtual bool store(FileEntityPtr pEntity, const std::string filepath, bool binary) = 0;

		virtual bool isOpen(FileEntityPtr pEntity) = 0;
		virtual bool isEof(FileEntityPtr pEntity) = 0;
		virtual bool open(FileEntityPtr pEntity, const std::string filepath, FileSystemResourceComponent::AccessMode mode, bool binary = true) = 0;
		virtual bool close(FileEntityPtr pEntity) = 0;

		virtual uint32_t write(FileEntityPtr pEntity, const std::string content) = 0;
		virtual uint32_t write(FileEntityPtr pEntity, const std::vector<uint8_t> bytes) = 0;
		virtual std::vector<uint8_t> read(FileEntityPtr pEntity, uint64_t byteCount) = 0;
		virtual std::string readLine(FileEntityPtr pEntity) = 0;
		virtual bool rewindFile(FileEntityPtr pEntity) = 0;



		~FileIOControllerBase();
	protected:
		FileIOControllerBase(const std::string identification);
	};

	typedef std::shared_ptr<FileIOControllerBase> FileIOControllerBasePtr;
}

#endif