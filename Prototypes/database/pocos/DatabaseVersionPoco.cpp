#include "DatabaseVersionPoco.h"

namespace crossforge {
	DatabaseVersionPoco::DatabaseVersionPoco() {
		clear();
	}
	DatabaseVersionPoco::~DatabaseVersionPoco() {
		clear();
	}

	bool DatabaseVersionPoco::initialize(const Json::Value json) {
		clear();
		bool result = false;
		try {
			if (!json["versionId"].isNull()) m_versionId = json["versionId"].asLargestInt();
			if (!json["major"].isNull()) m_major = json["major"].asInt();
			if (!json["minor"].isNull()) m_minor = json["minor"].asInt();
			if (!json["patch"].isNull()) m_patch = json["patch"].asInt();
			result = true;
		}
		catch (const std::exception& e) {
			LogError("Exception parsing json: " + std::string(e.what()));
		}
		return result;
	}
	void DatabaseVersionPoco::clear() {
		m_versionId = -1;
		m_major = -1;
		m_minor = -1;
		m_patch = -1;
	}
	Json::Value DatabaseVersionPoco::toJson()const {
		Json::Value result;
		result["versionId"] = m_versionId;
		result["major"] = m_major;
		result["minor"] = m_minor;
		result["patch"] = m_patch;
		return result;
	}

	/* Accessor */
	int64_t& DatabaseVersionPoco::versionId() {
		return m_versionId;
	}
	int32_t& DatabaseVersionPoco::major() {
		return m_major;
	}
	int32_t& DatabaseVersionPoco::minor() {
		return m_minor;
	}
	int32_t& DatabaseVersionPoco::patch() {
		return m_patch;
	}
}