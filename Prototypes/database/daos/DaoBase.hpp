/*****************************************************************************\
*                                                                           *
* File(s): DaoBase.hpp                  *
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
#ifndef __CROSSFORGE_DAOBASE_HPP__
#define __CROSSFORGE_DAOBASE_HPP__

#include <crossforge/core/CoreDefinitions.h>
#include "../entities/DatabaseConnectionEntity.h"
#include "../controllers/DatabaseConnectionEntityController.h"

namespace crossforge {
	template<typename T>
	class DaoBase {
	public:

		virtual void initialize(DatabaseConnectionEntityPtr pDatabaseConnection) {
			m_pDbConnection = pDatabaseConnection;
		}
		virtual void clear(){
			m_pDbConnection = nullptr;
		}

		virtual bool create(const std::shared_ptr<T> &pPoco) const = 0;
		virtual bool create(const std::vector<std::shared_ptr<T>> &pPocos) const = 0;

		virtual std::shared_ptr<T> read(const int64_t id) const = 0;
		virtual std::vector<std::shared_ptr<T>> readAll(int64_t limit = -1) const = 0;

		virtual bool update(const std::shared_ptr<T> &pPoco) const = 0;
		virtual bool update(const std::vector<std::shared_ptr<T>> &pPocos) const = 0;

		virtual bool erase(const int64_t id) const = 0;
		virtual bool erase(const std::vector<int64_t> &ids) const = 0;

		virtual const int64_t rowCount()const = 0;

		virtual ~DaoBase(){
			clear();
		};
	protected:
		DaoBase(std::string tableName, DatabaseConnectionEntityPtr pDbConnection){ 
			m_pDbConnection = pDbConnection;
			m_tableName = tableName;
		};
		
		DatabaseConnectionEntityPtr m_pDbConnection;
		std::string m_tableName;
	};
}

#endif
