/*****************************************************************************\
*                                                                           *
* File(s): Texture2DComponent.h and Texture2DComponent.cpp                       *
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
#ifndef __CROSSFORGE_TEXTURE2DCOMPONENT_H__
#define __CROSSFORGE_TEXTURE2DCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class Texture2DComponent : public ComponentBase {
	public:
		static inline std::string identification = "Texture2DComponent";

		Texture2DComponent();
		~Texture2DComponent();

		void initialize(const std::shared_ptr<const Texture2DComponent> pRef = nullptr);
		void clear();

		/** Accessor **/
		uint32_t& glTextureHandle();
		uint32_t& width();
		uint32_t& height();

		/** Getter **/
		const uint32_t getGlTextureHandle()const;
		const uint32_t getWidht()const;
		const uint32_t getHeight()const;

		/** Setter **/
		void setGlTextureHandle(const uint32_t handle);
		void setWidth(const uint32_t width);
		void setHeight(const uint32_t height);

	protected:
		Texture2DComponent(const std::string childIdentification);

		uint32_t m_glTextureHandle;
		uint32_t m_width;
		uint32_t m_height;
	};

	using Texture2DComponentPtr = std::shared_ptr<Texture2DComponent>;
	using Texture2DComponentCPtr = std::shared_ptr<const Texture2DComponent>;
}

#endif 