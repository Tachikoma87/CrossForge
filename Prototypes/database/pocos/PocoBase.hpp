/*****************************************************************************\
*                                                                           *
* File(s): PocoBase.h and PocoBase.cpp                       *
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
#ifndef __CROSSFORGE_POCOBASE_H__
#define __CROSSFORGE_POCOBASE_H__

#include <crossforge/core/CoreDefinitions.h>
#include <json/json.h>

namespace crossforge {
	template<typename T>
	class PocoBase {
	public:
		static Json::Value toJsonArray(const std::vector<T> items) {
			Json::Value result = Json::arrayValue;
			for (T item : items) result.append(item.toJson());
			return result;
		}
		static std::vector<T> fromJsonArray(const Json::Value items) {
			std::vector<T> result;
			for (Json::Value item : items) {
				T obj;
				obj.initialize(item);
				result.push_back(obj);
			}
			return result;
		}

		virtual bool initialize(const Json::Value json) = 0;
		virtual Json::Value toJson()const = 0;

		int64_t& timestampCreated() {
			return m_timestampCreated;
		}
		int64_t timestampModified() {
			return m_timestampModified;
		}
		
		virtual ~PocoBase() {

		}
	protected:
		PocoBase() {
			m_timestampCreated = -1;
			m_timestampModified = -1;
		}

		int64_t m_timestampCreated;
		int64_t m_timestampModified;
	};
}

#endif 