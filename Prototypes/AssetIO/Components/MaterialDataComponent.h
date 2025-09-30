/*****************************************************************************\
*                                                                           *
* File(s): MaterialDataComponent.h and MaterialDataComponent.cpp            *
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
#ifndef __CROSSFORGE_MATERIALDATACOMPONENT_H__
#define __CROSSFORGE_MATERIALDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>
#include "trianglemesh/MeshMaterial.h"

namespace crossforge {
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