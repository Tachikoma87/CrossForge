/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshEntity.h and TriangleMeshEntity.cpp                              *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachhikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_TRIANGLEMESHENTITY_H__
#define __CROSSFORGE_TRIANGLEMESHENTITY_H__

#include <crossforge/ecs/EntityBase.h>

#include "../Components/PositionDataComponent.h"
#include "../Components/NormalDataComponent.h"
#include "../Components/MeshDefinitionsComponent.h"
#include "../Components/TextureCoordinatesComponent.h"
#include "../components/MaterialDataComponent.h"

namespace crossforge {
	class TriangleMeshEntity : public EntityBase {
	public:
		inline static std::string identification = "TriangleMeshEntity";

		enum TriangleMeshComponents: uint8_t {
			POSITION_DATA_COMPONENT = 0x01,
			NORMAL_DATA_COMPONENT = 0x02,
			MESH_DEFINITIONS_COMPONENT = 0x04,
			TEXTURE_COORDINATES_COMPONENT = 0x08,
			MATERIAL_DATA_COMPONENT = 0x10,
			ALL_BASIC_COMPONENTS = 0xFF
		};

		TriangleMeshEntity(uint8_t componentBitmask = 0);
		~TriangleMeshEntity();

		void initialize(uint8_t compoonentBitmask);
		void clear();

		PositionDataComponentPtr getPositionDataComponent();
		NormalDataComponentPtr getNormalDataComponent();
		MeshDefinitionsComponentPtr getMeshDefinitionsComponent();
		TextureCoordinateComponentPtr getTextureCoordinatesComponent();
		MaterialDataComponentPtr getMaterialDataComponent();


	protected:

	};

	typedef std::shared_ptr<TriangleMeshEntity> TriangleMeshEntityPtr;
}

#endif