/*****************************************************************************\
*                                                                           *
* File(s): MeshDefinitionsComponent.h and MeshDefinitionsComponent.cpp            *
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
#ifndef __CFORGE_TRIANGLEDATACOMPONENT_H__
#define __CFORGE_TRIANGLEDATACOMPONENT_H__

#include <crossforge/Core/SLogger.h>
#include "../../ECS/ComponentBase.h"
#include "../TriangleMesh/MeshDefinition.h"


namespace CForge {
	class MeshDefinitionsComponent: public ComponentBase {
	public:
		inline static const std::string identification = "MeshDefinitionsComponent";

		MeshDefinitionsComponent();
		~MeshDefinitionsComponent();

		void initialize();
		void clear();

		void addMeshDefinition(MeshDefinitionPtr pMeshDefinition);
		MeshDefinitionPtr getMeshDefinition(uint32_t index);

		uint32_t getMeshDefinitionsCount()const;

	protected:
		std::vector<MeshDefinitionPtr> m_meshes;
	};

	typedef std::shared_ptr<MeshDefinitionsComponent> MeshDefinitionsComponentPtr;
}

#endif 