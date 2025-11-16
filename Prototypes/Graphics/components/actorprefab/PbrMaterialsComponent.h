/*****************************************************************************\
*                                                                           *
* File(s): PbrMaterialsComponent.h and PbrMaterialsComponent.cpp                      *
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
#ifndef __CROSSFORGE_PBRRMATERIALSCOMPONENT_H__
#define __CROSSFORGE_PBRRMATERIALSCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>
#include "../../entities/TextureEntity.h"
#include "../../entities/ShaderEntity.h"
#include "../uniformbuffer/UBOPBRMaterialComponent.h"

namespace crossforge {

	class PbrMaterial {
	public:
		enum TextureType : int8_t {
			TEXTURE_TYPE_UNKNOWN = -1,
			TEXTURE_TYPE_ALBEDO = 0,
			TEXTURE_TYPE_NORMAL,
			TEXTURE_TYPE_HEIGHT,
			TEXTURE_TYPE_METALLIC_ROUGHNESS,
			TEXTURE_TYPE_EMISSIVE,
			TEXTURE_TYPE_OCCLUSION,
			TEXTURE_TYPE_COUNT,
		};

		enum ColorType : int8_t {
			COLOR_TYPE_UNKNOWN,
			COLOR_TYPE_ALBEDO = 0,
			COLOR_TYPE_EMISSIVE,
			COLOR_TYPE_REFLECTIVE,
			COLOR_TYPE_SPECULAR,
			COLOR_TYPE_COUNT,
		};

	
		PbrMaterial();
		~PbrMaterial();

		void initialize(std::shared_ptr<PbrMaterial> pRef = nullptr);
		void clear();
		
		float& ambientOcclusion();
		float& metallic();
		float& roughness();
		Eigen::Vector4f& color(ColorType type);

		TextureEntityPtr& texture(TextureType type);

		UBOPBRMaterialComponentPtr uboPbrMaterial();
		bool updateUbo();

	protected:
		float m_ambientOcclusion;
		float m_metallic;
		float m_roughness;
		std::vector<Eigen::Vector4f> m_colors;
		std::vector<TextureEntityPtr> m_textures;
		UBOPBRMaterialComponentPtr m_pUbo;
	};
	typedef std::shared_ptr<PbrMaterial> PbrMaterialPtr;


	class PbrMaterialsComponent : public ComponentBase {
	public:
		static inline std::string identification = "PBRMaterialsComponent";

		PbrMaterialsComponent();
		~PbrMaterialsComponent();

		void initialize();
		void clear() override;


		std::vector<PbrMaterialPtr>& pbrMaterials();
		void addPbrMaterial(PbrMaterialPtr pMaterial);
		PbrMaterialPtr getMaterial(uint32_t index);
		uint32_t getMaterialCount()const;

	protected:
		PbrMaterialsComponent(const std::string childIdentification);

		std::vector<PbrMaterialPtr> m_pbrMaterials;

	};
	typedef std::shared_ptr<PbrMaterialsComponent> PbrMaterialsComponentPtr;
}

#endif 