/*****************************************************************************\
*                                                                           *
* File(s): SCrossForgeRestTestApp.h and SCrossForgeRestTestApp.cpp                       *
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
#ifndef __CROSSFORGE_SCROSSFORGERESTTESTAPP_H__
#define __CROSSFORGE_SCROSSFORGERESTTESTAPP_H__

#include <crossforge/core/ApplicationBase.h>
#include <crossforge/database/entities/DatabaseConnectionEntity.h>
#include <crossforge/database/controller/DatabaseConnectionEntityController.h>
#include <crossforge/database/provider/SDatabaseConnectionProvider.h>

namespace crossforge {
	class SCrossForgeRestTestApp : public ApplicationBase {
	public:
		static std::shared_ptr<SCrossForgeRestTestApp> instance();
		static void destroy();

		void initialize() override;
		void update() override;

		~SCrossForgeRestTestApp();
	protected:
		static std::shared_ptr<SCrossForgeRestTestApp> m_pInstance;

		SCrossForgeRestTestApp();

		void databaseMultiThreadTestFunc(int64_t index);
		bool databaseWriteTest(DatabaseConnectionEntityPtr pDbConnection, int64_t major, int64_t minor, int64_t patch, PostgresQueriesComponentPtr pBatchComponent);
		bool databaseReadTest(DatabaseConnectionEntityPtr pDbConnection, int64_t major, int64_t minor, int64_t patch, PostgresQueriesComponentPtr pBatchComponent);

		bool databaseDaoTest();

		DatabaseConnectionEntityPtr m_pDbConnection;
		DatabaseConnectionProviderPtr m_pDbConnectionProvider;

		static const int64_t m_threadCount = 4;
		uint64_t m_executionTimesWrite[m_threadCount];
		uint64_t m_executionTimesWriteBatch[m_threadCount];
		uint64_t m_executionTimesRead[m_threadCount];
		uint64_t m_executionTimesReadBatch[m_threadCount];
		

	};

	using CrossForgeRestTestAppPtr = std::shared_ptr<SCrossForgeRestTestApp>;
	using CrossForgeRestTestAppCPtr = std::shared_ptr<const SCrossForgeRestTestApp>;
}

#endif 