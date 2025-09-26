/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshIOSystemBase.h and TriangleMeshIOSystemBase.cpp      *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_TRIANGLEMESHIOSYSTEMBASE_H__
#define __CFORGE_TRIANGLEMESHIOSYSTEMBASE_H__

#include "../../ECS/SystemBase.h"
#include "../Entities/TriangleMeshEntity.h"

namespace CForge {
	class TriangleMeshIOSystemBase : public SystemBase {
	public:
		/**
		* \brief Available file operations.
		*/
		enum Operation : uint8_t {
			OP_LOAD = 0,	///< Loading.
			OP_STORE,		///< Storing.
		};

		virtual bool canAcceptFile(std::string filePath, const Operation operation)const = 0;
		virtual bool loadMesh(const std::string filePath, TriangleMeshEntityPtr pEntity) = 0;
		virtual bool storeMesh(const std::string filepath, TriangleMeshEntityPtr pEntity) = 0;

	protected:
		TriangleMeshIOSystemBase(std::string identification);
		~TriangleMeshIOSystemBase();
	};
}

#endif