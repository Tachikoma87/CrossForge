/*****************************************************************************\
*                                                                           *
* File(s): TextureCoordinatesComponent.h and TextureCoordinateComponent.cpp *
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
#ifndef __CROSSFORGE_TEXTURECOORDINATESCOMPONENT_H__
#define __CROSSFORGE_TEXTURECOORDINATESCOMPONENT_H__


#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class TextureCoordinateComponent : public ComponentBase {
	public:
		inline static std::string identification = "TextureCoordinateComponent";

		TextureCoordinateComponent();
		~TextureCoordinateComponent();

		void initialize();
		void clear();

		void setTextureCoordinates(std::vector<Eigen::Vector3f> textureCoordinates);
		void setTextureCoordinate(Eigen::Vector3f textureCoordinate, uint32_t index);
		std::vector<Eigen::Vector3f> getTextureCoordinates()const;
		std::vector<Eigen::Vector3f>& getTextureCoordinates();
		Eigen::Vector3f getTextureCoordinate(uint32_t index)const;
		Eigen::Vector3f& getTextureCordinate(uint32_t index);

		uint32_t getTextureCoorindatesCount()const;


	protected:
		std::vector<Eigen::Vector3f> m_textureCoordinates;
	};

	typedef std::shared_ptr<TextureCoordinateComponent> TextureCoordinateComponentPtr;

}

#endif 