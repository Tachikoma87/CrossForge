#include "DatabaseConnectionEntity.h"

namespace crossforge {

	DatabaseConnectionEntity::DatabaseConnectionEntity(): EntityBase(DatabaseConnectionEntity::identification) {

	}
	DatabaseConnectionEntity::DatabaseConnectionEntity(const std::string childIdentification): EntityBase(DatabaseConnectionEntity::identification){
		m_inheritance.push_back(childIdentification);
	}
	DatabaseConnectionEntity::~DatabaseConnectionEntity() {

	}

	PostgresConnectionComponentPtr DatabaseConnectionEntity::getPostgresConnectionComponent(const bool createIfNotExists) {
		return getComponent<PostgresConnectionComponent>(createIfNotExists);
	}
	DatabaseConnectionSettingsComponentPtr DatabaseConnectionEntity::getDatabaseConnectionSettingsComponent(const bool createIfNotExists) {
		return getComponent<DatabaseConnectionSettingsComponent>(createIfNotExists);
	}
	PostgresQueriesComponentPtr DatabaseConnectionEntity::getPostgresQueriesComponent(const bool createIfNotExists) {
		return getComponent<PostgresQueriesComponent>(createIfNotExists);
	}


	PostgresConnectionComponentCPtr DatabaseConnectionEntity::getPostgresConnectionComponent()const {
		return getComponent<PostgresConnectionComponent>();
	}
	DatabaseConnectionSettingsComponentCPtr DatabaseConnectionEntity::getDatabaseConnectionSettingsComponent()const {
		return getComponent<DatabaseConnectionSettingsComponent>();
	}
	PostgresQueriesComponentCPtr DatabaseConnectionEntity::getPostgresQueriesComponent()const {
		return getComponent<PostgresQueriesComponent>();
	}
}