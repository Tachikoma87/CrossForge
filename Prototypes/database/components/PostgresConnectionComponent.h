/*****************************************************************************\
*                                                                           *
* File(s): DatabaseConnectionComponent.h and DatabaseConnectionComponent.cpp                       *
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
#ifndef __CROSSFORGE_POSTGERSCONNECTIONCOMPONENT_H__
#define __CROSSFORGE_POSTGRESCONNECTIONCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>
#include <pqxx/pqxx>

namespace crossforge {
	class PostgresConnectionComponent : public ComponentBase {
	public:
		static inline std::string identification = "PostgresConnectionComponent";

		PostgresConnectionComponent();
		~PostgresConnectionComponent();

		void initialize(const std::shared_ptr<const PostgresConnectionComponent> pRef = nullptr);
		void clear();

		/* Accessor */
		std::shared_ptr<pqxx::connection>& connection();
		
		/* Getter */
		const std::shared_ptr<const pqxx::connection> getConnection()const;

		/* Setter */
		void setConnection(const std::shared_ptr<pqxx::connection> pConnection);

	protected:
		PostgresConnectionComponent(const std::string childIdentification);

		std::shared_ptr<pqxx::connection> m_pConnection;
	};

	using PostgresConnectionComponentPtr = std::shared_ptr<PostgresConnectionComponent>;
	using PostgresConnectionComponentCPtr = std::shared_ptr<const PostgresConnectionComponent>;

}

#endif 