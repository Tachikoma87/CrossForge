/*****************************************************************************\
*                                                                           *
* File(s): MeshMaterial.h and MeshMaterial.cpp            *
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
#ifndef __CFORGE_MESHMATERIAL_H__
#define __CFORGE_MESHMATERIAL_H__

#include <crossforge/Core/SLogger.h>

namespace CForge {
	class MeshMaterial {
	public:
		enum TextureType: uint8_t {
			TEXTURE_TYPE_AMBIENT = 0,
			TEXTURE_TYPE_DIFFUSE,
			TEXTURE_TYPE_NORMAL,
			TEXTURE_TYPE_HEIGHT,
			TEXTURE_TYPE_METALLIC_ROUGHNESS,
			TEXTURE_TYPE_EMISSIVE,
			TEXTURE_TYPE_OCCLUSION,
			TEXTURE_TYPE_COUNT
		};
		enum ColorType : uint8_t {
			COLOR_TYPE_AMBIENT = 0,
			COLOR_TYPE_DIFFUSE,
			COLOR_TYPE_EMISSIVE,
			COLOR_TYPE_REFLECTIVE,
			COLOR_TYPE_SPECULAR,
			COLOR_TYPE_TRANSPARENT,
			COLOR_TYPE_COUNT
		};


		MeshMaterial();
		~MeshMaterial();

		void initialize();
		void clear();

		std::string getTexturePath(TextureType type)const;
		std::string getTexturePath(TextureType type);
		Eigen::Vector4f getColor(ColorType type)const;
		Eigen::Vector4f getColor(ColorType type);
		float getMetallic()const;
		float getRoughness()const;
		float getShininess()const;

		void setTexturePath(std::string texturePath, TextureType type);
		void setColor(Eigen::Vector4f color, ColorType type);
		void setMetallica(float metallic);
		void setRoughness(float roughness);
		void setShininess(float shininess);

	protected:
		std::string m_texturePaths[TEXTURE_TYPE_COUNT];
		Eigen::Vector4f m_colors[COLOR_TYPE_COUNT];
		float m_metallic;
		float m_roughness;
		float m_shininess;

	};

	typedef std::shared_ptr<MeshMaterial> MeshMaterialPtr;
}

#endif 