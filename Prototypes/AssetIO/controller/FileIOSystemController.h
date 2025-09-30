/*****************************************************************************\
*                                                                           *
* File(s): FileIOSystemController and FileIOSystemController.cpp      *
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
#ifndef __CROSSFORGE_FILEIOSYSTEMCONTROLLER_H__
#define __CROSSFORGE_FILEIOSYSTEMCONTROLLER_H__

#include "FileIOControllerBase.h"

namespace crossforge {
	class FileIOSystemController : public FileIOControllerBase {
	public:
		inline static std::string identification = "FileIOSystemController";

		FileIOSystemController();
		~FileIOSystemController();

		virtual bool load(FileEntityPtr pEntity, const std::string filepath, bool binary);
		virtual bool store(FileEntityPtr pEntity, const std::string filepath, bool binary);

		virtual bool isOpen(FileEntityPtr pEntity);
		virtual bool isEof(FileEntityPtr pEntity);
		virtual bool open(FileEntityPtr pEntity, const std::string filepath, FileSystemResourceComponent::AccessMode mode, bool binary = true);
		virtual bool close(FileEntityPtr pEntity);

		virtual uint32_t write(FileEntityPtr pEntity, const std::string content);
		virtual uint32_t write(FileEntityPtr pEntity, const std::vector<uint8_t> bytes);
		virtual std::vector<uint8_t> read(FileEntityPtr pEntity, uint64_t byteCount);
		virtual std::string readLine(FileEntityPtr pEntity);
		virtual bool rewindFile(FileEntityPtr pEntity);

		
	protected:
		FileIOSystemController(const std::string childIdentification);
	};

	typedef std::shared_ptr<FileIOSystemController> FileIOSystemControllerPtr;
}

#endif 