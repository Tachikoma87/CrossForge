#include "PostgresQueriesComponent.h"

namespace crossforge {
	PostgresQuery::PostgresQuery() {
		initialize();
	}
	PostgresQuery::~PostgresQuery() {
		clear();
	}

	void PostgresQuery::initialize() {
		clear();
	}
	void PostgresQuery::clear() {
		m_query = "";
		m_params = pqxx::params();
		m_result = pqxx::result();

		m_wasExecuted = false;
		m_errorMessage = "";
	}

	/* Accessor */
	std::string& PostgresQuery::query() {
		return m_query;
	}
	pqxx::params& PostgresQuery::params() {
		return m_params;
	}
	pqxx::result& PostgresQuery::result() {
		return m_result;
	}
	bool& PostgresQuery::wasExecuted() {
		return m_wasExecuted;
	}
	std::string& PostgresQuery::errorMessage() {
		return m_errorMessage;
	}

	/* Getter */
	const std::string PostgresQuery::getQuery()const {
		return m_query;
	}
	const pqxx::params PostgresQuery::getParams()const {
		return m_params;
	}
	const pqxx::result PostgresQuery::getResult()const {
		return m_result;
	}
	const bool PostgresQuery::getWasExecuted()const {
		return m_wasExecuted;
	}
	const std::string PostgresQuery::getErrorMessage()const {
		return m_errorMessage;
	}

	/* Setter */
	void PostgresQuery::setQuery(const std::string query) {
		m_query = query;
	}
	void PostgresQuery::setParams(const pqxx::params params) {
		m_params = params;
	}
	void PostgresQuery::setResult(const pqxx::result result) {
		m_result = result;
	}
	void PostgresQuery::setWasExecuted(const bool wasExecuted) {
		m_wasExecuted = wasExecuted;
	}
	void PostgresQuery::setErrorMessage(const std::string errorMessage) {
		m_errorMessage = errorMessage;
	}






	PostgresQueriesComponent::PostgresQueriesComponent(): ComponentBase(PostgresQueriesComponent::identification) {
		initialize();
	}
	PostgresQueriesComponent::~PostgresQueriesComponent() {
		clear();
	}

	PostgresQueriesComponent::PostgresQueriesComponent(const std::string childIdentification): ComponentBase(PostgresQueriesComponent::identification) {
		m_inheritance.push_back(childIdentification);
	}

	void PostgresQueriesComponent::initialize(const std::shared_ptr<const PostgresQueriesComponent> pRef) {
		if(this != pRef.get()) clear();
		if (nullptr != pRef) {
			m_postgresQueries = pRef->m_postgresQueries;
		}
	}
	void PostgresQueriesComponent::clear() {
		m_postgresQueries.clear();
	}

	/* Accessor */
	std::vector<PostgresQueryPtr>& PostgresQueriesComponent::postgresQueries() {
		return m_postgresQueries;
	}

	/* Getter */
	const std::vector<PostgresQueryCPtr> PostgresQueriesComponent::getPostgresQueries()const {
		std::vector<PostgresQueryCPtr> result;
		for (auto pQuery : m_postgresQueries) result.push_back(pQuery);
		return result;
	}

	/* Setter */
	void PostgresQueriesComponent::setPostgresQueries(std::vector<PostgresQueryPtr> queries) {
		m_postgresQueries = queries;
	}


}