/*****************************************************************************\
*                                                                           *
* File(s): DatabaseConnectionSettingsComponent.h and DatabaseConnectionComponent.cpp                       *
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
#ifndef __CROSSFORGE_DATABASECONNECTIONSETTINGSCOMPONENT_H__
#define __CROSSFORGE_DATABASECONNECTIONSETTINGSCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge{
	class DatabaseConnectionSettingsComponent : public ComponentBase {
	public:
		static inline std::string identification = "DatabaseConnectionSettingsComponent";

		DatabaseConnectionSettingsComponent();
		~DatabaseConnectionSettingsComponent();

		void initialize(const std::shared_ptr<const DatabaseConnectionSettingsComponent> pRef = nullptr);
		void clear();

		/* Accessor */
		std::string& databaseName();
		std::string& userName();
		std::string& userPassword();
		std::string& hostAddress();
		uint16_t& port();

		/* Getter */
		const std::string getDatabaseName() const;
		const std::string getUserName() const;
		const std::string getUserPassword() const;
		const std::string getHostAddress() const;
		const uint16_t getPort()const;

		/* Setter */
		void setDatabaseName(const std::string databaseName);
		void setUserName(const std::string userName);
		void setUserPassword(const std::string userPassword);
		void setHostAddress(const std::string hostAddress);
		void setPort(const uint16_t port);


	protected:
		DatabaseConnectionSettingsComponent(const std::string childIdentification);

		std::string m_databaseName;
		std::string m_userName;
		std::string m_userPassword;

		std::string m_hostAdress;
		uint16_t m_port;

	};

	using DatabaseConnectionSettingsComponentPtr = std::shared_ptr<DatabaseConnectionSettingsComponent>;
	using DatabaseConnectionSettingsComponentCPtr = std::shared_ptr<const DatabaseConnectionSettingsComponent>;
}

#endif 