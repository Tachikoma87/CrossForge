/*****************************************************************************\
*                                                                           *
* File(s): DatabaseConnectionentity.h and DatabaseConnectionEntity.cpp                       *
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
#ifndef __CROSSFORGE_DATABASECONNECTIONENTITY_H__
#define __CROSSFORGE_DATABASECONNECTIONENTITY_H__

#include <crossforge/ecs/EntityBase.h>
#include "../components/PostgresConnectionComponent.h"
#include "../components/DatabaseConnectionSettingsComponent.h"
#include "../components/PostgresQueriesComponent.h"

namespace crossforge {
	class DatabaseConnectionEntity : public EntityBase {
	public:
		static inline std::string identification = "DatabaseConnectionEntity";

		DatabaseConnectionEntity();
		~DatabaseConnectionEntity();

		PostgresConnectionComponentPtr getPostgresConnectionComponent(const bool createIfNotExists = false);
		DatabaseConnectionSettingsComponentPtr getDatabaseConnectionSettingsComponent(const bool createIfNotExists = false);
		PostgresQueriesComponentPtr getPostgresQueriesComponent(const bool createIfNotExists = false);

		PostgresConnectionComponentCPtr getPostgresConnectionComponent()const;
		DatabaseConnectionSettingsComponentCPtr getDatabaseConnectionSettingsComponent()const;
		PostgresQueriesComponentCPtr getPostgresQueriesComponent()const;

	protected:
		DatabaseConnectionEntity(const std::string childIdentification);

	};

	using DatabaseConnectionEntityPtr = std::shared_ptr<DatabaseConnectionEntity>;
	using DatabaseConnectionEntityCPtr = std::shared_ptr<const DatabaseConnectionEntity>;
}

#endif 