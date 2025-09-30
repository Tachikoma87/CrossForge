#include "FileEntity.h"

namespace crossforge {

	FileEntity::FileEntity(uint8_t componentBitmask): EntityBase(FileEntity::identification) {
		m_inheritance.push_back(FileEntity::identification);
		initialize(componentBitmask);
	}
	FileEntity::~FileEntity() {
		clear();
	}

	void FileEntity::initialize(uint8_t componentBitmask) {
		clear();
		if (componentBitmask & BINARY_DATA_COMPONENT) addComponent(std::make_shared<BinaryDataComponent>());
		if (componentBitmask & FILE_SYSTEM_RESOURCE_COMPONENT) addComponent(std::make_shared<FileSystemResourceComponent>());
		if (componentBitmask & STRING_DATA_COMPONENT) addComponent(std::make_shared<StringDataComponent>());

	}
	void FileEntity::clear() {
		m_componentMap.clear();
	}

	BinaryDataComponentPtr FileEntity::getBinaryDataComponent() {
		return getComponent<BinaryDataComponent>();
	}
	FileSystemResourceComponentPtr FileEntity::getFileSystemResourceComponent() {
		return getComponent<FileSystemResourceComponent>();
	}
	StringDataComponentPtr FileEntity::getStringDataComponent() {
		return getComponent<StringDataComponent>();
	}
}