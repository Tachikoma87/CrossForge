#include "FileSystemResourceComponent.h"

namespace crossforge {
	FileSystemResourceComponent::FileSystemResourceComponent() : ComponentBase(FileSystemResourceComponent::identification) {
		m_inheritance.push_back(FileSystemResourceComponent::identification);
		m_pHandle = nullptr;
	}

	FileSystemResourceComponent::~FileSystemResourceComponent() {
		clear();
	}

	void FileSystemResourceComponent::initialize() {
		clear();
	}
	void FileSystemResourceComponent::clear() {
		if (nullptr != m_pHandle) LogWarning("Found file handle with non nullptr value. Did you close all files correclty?");
		// @Todo: Try to close file
		m_pHandle = nullptr;
		m_originalFilepath = "";
		m_accessMode = MODE_UNKNOWN;
		m_binaryMode = false;
	}

	std::unique_ptr<FILE*> &FileSystemResourceComponent::handle() {
		return m_pHandle;
	}

	bool& FileSystemResourceComponent::binaryMode() {
		return m_binaryMode;
	}
	FileSystemResourceComponent::AccessMode& FileSystemResourceComponent::accessMode() {
		return m_accessMode;
	}
	std::string& FileSystemResourceComponent::originalFilepath() {
		return m_originalFilepath;
	}
};