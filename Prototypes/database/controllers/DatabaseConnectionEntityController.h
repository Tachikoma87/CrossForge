/*****************************************************************************\
*                                                                           *
* File(s): DatabaseConnectionEntityController.h and DatabaseConnectionEntityController.cpp                       *
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
#ifndef __CROSSFORGE_DATABASECONNECTIONENTITYCONTROLLER_H__
#define __CROSSFORGE_DATABASECONNECTIONENTITYCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include "../entities/DatabaseConnectionEntity.h"

namespace crossforge {
	class DatabaseConnectionEntityController : public ControllerBase {
	public:
		static inline std::string identification = "DatabaseConnectionEntityController";

		static bool openPostgresConnection(DatabaseConnectionEntityPtr pDatabaseConnection);
		static bool closePostgresConnection(DatabaseConnectionEntityPtr pDatabaseConnection);
		static bool isPostgresConnectionOpen(DatabaseConnectionEntityCPtr pDatabaseConnection);

		static bool executePostgresQuery(DatabaseConnectionEntityPtr pDatabaseConnection, PostgresQueryPtr pQuery);
		static bool executePostgresQueries(DatabaseConnectionEntityPtr pDatabaseConnection);


		static std::shared_ptr<pqxx::transaction<>> createPostgresTransaction(DatabaseConnectionEntityPtr pDatabaseConnection);
		static std::shared_ptr<pqxx::nontransaction> createPostgresNonTransaction(DatabaseConnectionEntityPtr pDatabaseConnection);


		~DatabaseConnectionEntityController();
	protected:
		DatabaseConnectionEntityController(const std::string identification);

	};

	using DatabaseConnectionEntityControllerPtr = std::shared_ptr<DatabaseConnectionEntityController>;
	using DatabaseConnectionEntityControllerCPtr = std::shared_ptr<const DatabaseConnectionEntityController>;
}

#endif 