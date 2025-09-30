#include "../../utility/FileUtility.h"
#include "FileIOSystemController.h"

namespace crossforge {

	FileIOSystemController::FileIOSystemController(): FileIOControllerBase(FileIOSystemController::identification) {

	}
	FileIOSystemController::~FileIOSystemController() {

	}
	FileIOSystemController::FileIOSystemController(const std::string childIdentification): FileIOControllerBase(FileIOSystemController::identification) {
		m_inheritance.push_back(childIdentification);
	}

	bool FileIOSystemController::load(FileEntityPtr pEntity, const std::string filepath, bool binary) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = false;
		if (!FileUtility::exists(filepath)) {
			LogError("File " + filepath + " does not exist. Can not load!");
		}
		else if (!open(pEntity, filepath, FileSystemResourceComponent::MODE_READ, binary)) {
			LogError("Failed to open file: " + filepath);
		}
		else {
			int64_t fileSize = FileUtility::size(filepath);
			std::vector<uint8_t> content = read(pEntity, fileSize);
			close(pEntity);
			if (!binary) {
				content.push_back('\0');
				if (!pEntity->hasComponent(StringDataComponent::identification)) pEntity->addComponent(std::make_shared<StringDataComponent>());
				pEntity->getStringDataComponent()->stringData() = (char*)content.data();
			}
			else {
				if (!pEntity->hasComponent(BinaryDataComponent::identification)) pEntity->addComponent(std::make_shared<BinaryDataComponent>());
				pEntity->getBinaryDataComponent()->binaryData() = content;
			}
			result = true;
		}
		return result;
	}
	bool FileIOSystemController::store(FileEntityPtr pEntity, const std::string filepath, bool binary) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		bool result = false;
		// create the file
		if (!open(pEntity, filepath, FileSystemResourceComponent::MODE_WRITE, binary)) {
			LogError("Failed to open or create file " + filepath);
		}
		else if(binary){
			auto pBinaryData = pEntity->getBinaryDataComponent();
			if (nullptr == pBinaryData) {
				LogError("Entity has no binary data component. Can not write meaningful data to " + filepath);
			}
			else {
				write(pEntity, pBinaryData->binaryData());
			}
		}
		else {
			auto pStringDataComp = pEntity->getStringDataComponent();
			if (nullptr == pStringDataComp) {
				LogError("Entity has no string data component. Can not write meaningful data to " + filepath);
			}
			else {
				write(pEntity, pStringDataComp->stringData());
			}
		}

		if (isOpen(pEntity)) close(pEntity);

		return result;
	}

	bool FileIOSystemController::isOpen(FileEntityPtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		auto pResourceComp = pEntity->getFileSystemResourceComponent();
		return (nullptr != pResourceComp && nullptr != pResourceComp->handle());
	}
	bool FileIOSystemController::isEof(FileEntityPtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		auto pResourceComp = pEntity->getFileSystemResourceComponent();
		bool result = true;
		if (nullptr != pResourceComp && nullptr != pResourceComp->handle()) {
			result = (0 != feof(*pResourceComp->handle()));
		}
		return result;
	}
	bool FileIOSystemController::open(FileEntityPtr pEntity, const std::string filepath, FileSystemResourceComponent::AccessMode mode, bool binary) {

		std::string modeString = "";
		switch (mode) {
		case FileSystemResourceComponent::MODE_READ: modeString = (binary) ? "rb" : "r"; break;
		case FileSystemResourceComponent::MODE_READ_EXTENDED: modeString = (binary) ? "r+b" : "r+"; break;
		case FileSystemResourceComponent::MODE_WRITE: modeString = (binary) ? "wb" : "w"; break;
		case FileSystemResourceComponent::MODE_WRITE_EXTENED: modeString = (binary) ? "w+b" : "w+"; break;
		case FileSystemResourceComponent::MODE_APPEND: modeString = (binary) ? "ab" : "a"; break;
		case FileSystemResourceComponent::MODE_APPEND_EXTENDED: modeString = (binary) ? "a+b" : "a+"; break;
		default: {
			LogError("Unrecognized access mode " + std::to_string(mode) + ". Can not open file " + filepath);
			return false;
		}break;
		}

		bool result = false;
		// make sure directory exists
		std::string directory = FileUtility::removeFilename(filepath);
		if (!FileUtility::exists(filepath) && !FileUtility::isDirectory(directory) && !FileUtility::createDirectories(directory)) {
			LogError("Can not create directory " + directory + " for file " + filepath);
		}
		else {
			FILE* pF = fopen(filepath.c_str(), modeString.c_str());
			if (nullptr == pF) LogError("Failed to open file: " + filepath);
			else {
				if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) pEntity->addComponent(std::make_shared<FileSystemResourceComponent>());
				auto pFileResourceComp = pEntity->getFileSystemResourceComponent();
				pFileResourceComp->accessMode() = mode;
				pFileResourceComp->binaryMode() = binary;
				pFileResourceComp->handle() = std::make_unique<FILE*>(pF);
				result = true;
			}
		}
		return result;
	}
	bool FileIOSystemController::close(FileEntityPtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) throw MissingComponentException(FileSystemResourceComponent::identification);
		auto pFileHandle = *pEntity->getFileSystemResourceComponent()->handle();
		fclose(&(*pFileHandle));
		pEntity->getFileSystemResourceComponent()->handle() = nullptr;
		pEntity->removeComponent(FileSystemResourceComponent::identification);
		return true;
	}

	uint32_t FileIOSystemController::write(FileEntityPtr pEntity, const std::string content) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) throw MissingComponentException(FileSystemResourceComponent::identification);

		uint32_t result = 0;
		FILE* pFile = *pEntity->getFileSystemResourceComponent()->handle();
		if (nullptr == pFile) {
			LogError("File entity has no valid file handle. can not write data!");
		}
		else {
			result = fwrite(content.c_str(), sizeof(char), content.length(), pFile);
		}
		return result;
	}
	uint32_t FileIOSystemController::write(FileEntityPtr pEntity, const std::vector<uint8_t> bytes) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) throw MissingComponentException(FileSystemResourceComponent::identification);

		uint32_t result = 0;
		FILE* pFile = *pEntity->getFileSystemResourceComponent()->handle();
		if (nullptr == pFile) {
			LogError("File entity has no valid file handle. can not write data!");
		}
		else {
			result = fwrite(bytes.data(), sizeof(uint8_t), bytes.size(), pFile);
		}
		return result;
	}
	std::vector<uint8_t> FileIOSystemController::read(FileEntityPtr pEntity, uint64_t byteCount) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) throw MissingComponentException(FileSystemResourceComponent::identification);
		std::vector<uint8_t> result;
		FILE* pFile = *pEntity->getFileSystemResourceComponent()->handle();
		if (nullptr == pFile) {
			LogError("File entity has no valid file handle. can not write data!");
		}
		else {
			result.resize(byteCount);
			uint32_t bytesRead = fread(result.data(), sizeof(uint8_t), byteCount, pFile);
			if (bytesRead != byteCount) result.resize(bytesRead);	
		}
		return result;
	}
	std::string FileIOSystemController::readLine(FileEntityPtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		if (!pEntity->hasComponent(FileSystemResourceComponent::identification)) throw MissingComponentException(FileSystemResourceComponent::identification);
		std::string result = "";
		FILE* pFile = *pEntity->getFileSystemResourceComponent()->handle();
		if (nullptr == pFile) {
			LogError("File " + pEntity->getFileSystemResourceComponent()->originalFilepath() + " has no valid file handle. Can not read data!");
		}
		else {
			const uint8_t bufferSize = 32;
			char buffer[bufferSize];
			char* pResultString = fgets(buffer, bufferSize, pFile);
			while (nullptr != pResultString) {		
				buffer[bufferSize-1] = '\0'; // just to be safe
				result += buffer;
				if (result.find('\n') != std::string::npos) break;
				pResultString = fgets(buffer, bufferSize, pFile);	
			}	
		}
		return result;

	}
	bool FileIOSystemController::rewindFile(FileEntityPtr pEntity) {
		if (nullptr == pEntity) throw NullpointerExcept("pEntity");
		auto pResourceComp = pEntity->getFileSystemResourceComponent();
		bool result = false;
		if (nullptr != pResourceComp && nullptr != pResourceComp->handle()) {
			rewind(*pResourceComp->handle());
			result = true;
		}
		return result;
	}

}