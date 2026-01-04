/*****************************************************************************\
*                                                                           *
* File(s): PostgresQueriesComponent.h and PostgresQueriesComponent.cpp                       *
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
#ifndef __CROSSFORGE_POSTGRESQUERIESCOMPONENT_H__
#define __CROSSFORGE_POSTGRESQUERIESCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>
#include <pqxx/pqxx>

namespace crossforge {

	class PostgresQuery {
	public:
		PostgresQuery();
		~PostgresQuery();

		void initialize();
		void clear();

		/* Accessor */
		std::string& query();
		pqxx::params& params();
		pqxx::result& result();
		bool& wasExecuted();
		std::string& errorMessage();

		/* Getter */
		const std::string getQuery()const;
		const pqxx::params getParams()const;
		const pqxx::result getResult()const;
		const bool getWasExecuted()const;
		const std::string getErrorMessage()const;

		/* Setter */
		void setQuery(const std::string query);
		void setParams(const pqxx::params params);
		void setResult(const pqxx::result result);
		void setWasExecuted(const bool wasExecuted);
		void setErrorMessage(const std::string errorMessage);

	protected:
		std::string m_query;
		pqxx::params m_params;
		pqxx::result m_result;

		bool m_wasExecuted;
		std::string m_errorMessage;
	};

	using PostgresQueryPtr = std::shared_ptr<PostgresQuery>;
	using PostgresQueryCPtr = std::shared_ptr<const PostgresQuery>;


	class PostgresQueriesComponent : public ComponentBase {
	public:
		static inline std::string identification = "PostgresQueriesComponent";

		PostgresQueriesComponent();
		~PostgresQueriesComponent();

		void initialize(const std::shared_ptr<const PostgresQueriesComponent> pRef = nullptr);
		void clear();

		/* Accessor */
		std::vector<PostgresQueryPtr>& postgresQueries();

		/* Getter */
		const std::vector<PostgresQueryCPtr> getPostgresQueries()const;

		/* Setter */
		void setPostgresQueries(std::vector<PostgresQueryPtr> queries);

	protected:
		PostgresQueriesComponent(const std::string childIdentification);

		std::vector<PostgresQueryPtr> m_postgresQueries;

	};

	using PostgresQueriesComponentPtr = std::shared_ptr<PostgresQueriesComponent>;
	using PostgresQueriesComponentCPtr = std::shared_ptr<const PostgresQueriesComponent>;
}

#endif 