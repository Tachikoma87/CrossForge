/*****************************************************************************\
*                                                                           *
* File(s): DatabaseVersionPoco.h and DatabaseVersionPoco.cpp                       *
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
#ifndef __CROSSFORGE_DATABASEVERSIONPOCO_H__
#define __CROSSFORGE_DATABASEVERSIONPCOO_H__

#include "PocoBase.hpp"

namespace crossforge {
	class DatabaseVersionPoco : public PocoBase<DatabaseVersionPoco> {
	public:
		DatabaseVersionPoco();
		~DatabaseVersionPoco();

		bool initialize(const Json::Value json) override;
		void clear();
		Json::Value toJson()const override;

		/* Accessor */
		int64_t& versionId();
		int32_t& major();
		int32_t& minor();
		int32_t& patch();

	protected:
		int64_t m_versionId;
		int32_t m_major;
		int32_t m_minor;
		int32_t m_patch;
	};

	using DatabaseVersionPocoPtr = std::shared_ptr<DatabaseVersionPoco>;
	using DatabaseVersionPocoCPtr = std::shared_ptr<const DatabaseVersionPoco>;
}

#endif