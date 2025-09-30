/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshIOControllerBase.h and TriangleMeshIOControllerBase.cpp      *
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
#ifndef __CROSSFORGE_TRIANGLEMESHIOCONTROLLERBASE_H__
#define __CROSSFORGE_TRIANGLEMESHIOCONTROLLERBASE_H__

#include "../../ECS/ControllerBase.h"
#include "../entities/TriangleMeshEntity.h"

namespace crossforge {
	class TriangleMeshIOControllerBase : public ControllerBase {
	public:
		inline static std::string identification = "TriangleMeshIOControllerBase";

		/**
		* \brief Available file operations.
		*/
		enum Operation : uint8_t {
			OP_LOAD = 0,	///< Loading.
			OP_STORE,		///< Storing.
		};

		virtual bool canAcceptFile(std::string filePath, const Operation operation)const = 0;
		virtual bool load(TriangleMeshEntityPtr pEntity, const std::string filePath) = 0;
		virtual bool store(TriangleMeshEntityPtr pEntity, const std::string filepath) = 0;

		~TriangleMeshIOControllerBase();
	protected:
		TriangleMeshIOControllerBase(std::string childIdentification);
		
	};

	typedef std::shared_ptr<TriangleMeshIOControllerBase> TriangleMeshIOControllerBasePtr;
}

#endif