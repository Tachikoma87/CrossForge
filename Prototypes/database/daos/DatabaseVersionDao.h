/*****************************************************************************\
*                                                                           *
* File(s): DatabaseVersionDao.h and DatabaseVersionDao.cpp                  *
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
#ifndef __CROSSFORGE_DATABASEVERSIONDAO_H__
#define __CROSSFORGE_DATABASEVERSIONDAO_H__

#include "DaoBase.hpp"
#include "../pocos/DatabaseVersionPoco.h"

namespace crossforge {
	class DatabaseVersionDao: public DaoBase<DatabaseVersionPoco> {
	public:
		DatabaseVersionDao(DatabaseConnectionEntityPtr pDbConnection = nullptr);
		~DatabaseVersionDao();

		bool create(const DatabaseVersionPocoPtr &pPoco) const override;
		bool create(const std::vector<DatabaseVersionPocoPtr> &pPocos) const override;

		DatabaseVersionPocoPtr read(const int64_t id) const override;
		std::vector<DatabaseVersionPocoPtr> readAll(int64_t limit = -1) const override;

		bool update(const DatabaseVersionPocoPtr &pPoco) const override;
		bool update(const std::vector<DatabaseVersionPocoPtr> &pPocos) const override;

		bool erase(const int64_t id) const override;
		bool erase(const std::vector<int64_t> &ids) const override;
		
		const int64_t rowCount()const override;

	protected:
		
		const std::vector<DatabaseVersionPocoPtr> parseResultSet(const pqxx::result resultSet) const;
	};

	using DatabaseVersionDaoPtr = std::shared_ptr<DatabaseVersionDao>;
	using DatabaseVersionDaoCPtr = std::shared_ptr<const DatabaseVersionDao>;

}

#endif