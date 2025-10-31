/*****************************************************************************\
*                                                                           *
* File(s): ActorInstanceEntity.h and ActorInstanceEntity.cpp                *
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
#ifndef __CROSSFORGE_ACTORINSTANCEENTITY_H__
#define __CROSSFORGE_ACTORINSTANCEENTITY_H__

#include "SceneObjectEntity.h"
#include "../components/Transformation3DComponent.h"
#include "../../Graphics/components/uniformbuffer/UBOTransformationDataComponent.h"

namespace crossforge {
	class ActorInstanceEntity : public SceneObjectEntity {
	public:
		static inline std::string identification = "ActorInstanceEntity";

		enum ActorInstanceComponents: uint8_t {
			COMPONENT_TRANSFORMATION_3D			= 0x01,
			COMPONENT_UBO_TRANSFORMATION_DATA	= 0x02,
			COMPONENT_MOVEMENT_3D = 0x04,
			COMPONENTS_ALL = 0xFF
		};

		ActorInstanceEntity(uint8_t componentsBitmask);
		~ActorInstanceEntity();

		void initialize(uint8_t componentsBitmask);
		void clear();

		UBOTransformationDataComponentPtr getUboTransformationDataComponent();

	protected:
		ActorInstanceEntity(const std::string childIdentification);

	};
	typedef std::shared_ptr<ActorInstanceEntity> ActorInstanceEntityPtr;
}

#endif 