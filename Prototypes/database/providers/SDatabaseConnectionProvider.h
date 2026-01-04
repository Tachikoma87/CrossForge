/*****************************************************************************\
*                                                                           *
* File(s): SDatabaseConnectionProvider.h and SDatabaseConnectionProvider.cpp                       *
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
#ifndef __CROSSFORGE_SDATABASECONNECTIONPROVIDER_H__
#define __CROSSFORGE_SDATABASECONNECTIONPROVIDER_H__

#include <thread>
#include <mutex>
#include <crossforge/core/CoreDefinitions.h>
#include "../entities/DatabaseConnectionEntity.h"

namespace crossforge {
	class SDatabaseConnectionProvider {
	public:
		static inline std::string identification = "SDatabaseConnectionProvider";

		static std::shared_ptr<SDatabaseConnectionProvider> instance();
		static void destroy();

		bool registerDatabaseConnection(std::string connectionName, DatabaseConnectionEntityPtr pDatabaseConnection);
		bool unregisterDatabaseConnection(std::string connectionName);
		bool isConnectionRegistered(std::string connectionName);

		DatabaseConnectionEntityPtr getDatabaseConnection(std::string connectionName);


		~SDatabaseConnectionProvider();
	protected:
		static std::shared_ptr<SDatabaseConnectionProvider> m_pInstance;
		static std::mutex m_mutex;

		std::unordered_map<std::string, DatabaseConnectionEntityPtr> m_registeredConnectionsMap;
		std::unordered_map<std::string, std::vector<DatabaseConnectionEntityPtr>> m_managedConnections;
		
		SDatabaseConnectionProvider();

	};

	using DatabaseConnectionProvider = SDatabaseConnectionProvider;
	using DatabaseConnectionProviderPtr = std::shared_ptr<SDatabaseConnectionProvider>;
	using DatabaseConnectionProviderCPtr = std::shared_ptr<const SDatabaseConnectionProvider>;
}

#endif 