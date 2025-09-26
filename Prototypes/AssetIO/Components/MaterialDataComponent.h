/*****************************************************************************\
*                                                                           *
* File(s): MaterialDataComponent.h and MaterialDataComponent.cpp            *
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
#ifndef __CFORGE_MATERIALDATACOMPONENT_H__
#define __CFORGE_MATERIALDATACOMPONENT_H__

#include <crossforge/Core/SLogger.h>
#include <crossforge/ecs/ComponentBase.h>
//#include "../../ECS/ComponentBase.h"
#include "../TriangleMesh/MeshMaterial.h"

namespace CForge {
	class MaterialDataComponent : public crossforge::ComponentBase {
	public:
		inline static std::string identification = "MaterialDataComponent";


		MaterialDataComponent();
		~MaterialDataComponent();

		void initialize();
		void clear();

		std::vector<MeshMaterialPtr>& getMaterials();
		std::vector<MeshMaterialPtr> getMaterials()const;
		MeshMaterialPtr getMaterial(uint32_t index);
		void setMaterial(MeshMaterialPtr pMaterial, uint32_t index);

		void addMaterial(MeshMaterialPtr pMaterial);
		void removeMaterial(uint32_t index);

		uint32_t getMaterialCount()const;

	protected:
		std::vector<MeshMaterialPtr> m_materials;
	};

	typedef std::shared_ptr<MaterialDataComponent> MaterialDataComponentPtr;
}

#endif