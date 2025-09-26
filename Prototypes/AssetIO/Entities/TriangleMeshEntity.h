/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshEntity.h and TriangleMeshEntity.cpp                              *
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
#ifndef __CFORGE_TRIANGLEMESHENTITY_H__
#define __CFORGE_TRIANGLEMESHENTITY_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/EntityBase.h"

#include "../Components/PositionDataComponent.h"
#include "../Components/NormalDataComponent.h"
#include "../Components/MeshDefinitionsComponent.h"
#include "../Components/TextureCoordinatesComponent.h"

namespace CForge {
	class TriangleMeshEntity : public EntityBase {
	public:
		inline static std::string identification = "TriangleMeshEntity";

		TriangleMeshEntity();
		~TriangleMeshEntity();

		void initialize();
		void clear();

		PositionDataComponentPtr getPositionDataComponent();
		NormalDataComponentPtr getNormalDataComponent();
		MeshDefinitionsComponentPtr getMeshDefinitionsComponent();
		TextureCoordinateComponentPtr getTextureCoordinatesComponent();


	protected:

	};

	typedef std::shared_ptr<TriangleMeshEntity> TriangleMeshEntityPtr;
}

#endif