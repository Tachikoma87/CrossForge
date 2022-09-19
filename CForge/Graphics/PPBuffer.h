#ifndef __CFORGE_PPBUFFER_H__
#define __CFORGE_PPBUFFER_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T2DImage.hpp"

namespace CForge{ 
	/**
	* \brief Post-Processing Buffer
	*/
	class CFORGE_API PPBuffer: public CForgeObject {
	public:
		enum Component: int8_t {
			COMP = 0
		};

		PPBuffer(void);
		~PPBuffer(void);

		void init(uint32_t Width, uint32_t Height);
		void clear(void);

		void bind(void)const;
		void unbind(void)const;

		void bindTexture(Component Comp, uint32_t Level);
		void blitDepthBuffer(uint32_t Width, uint32_t Height);
		void blitColorBuffer(uint32_t Width, uint32_t Height);

		uint32_t width(void)const;
		uint32_t height(void)const;
		
		uint32_t getFramebufferIndex();
		

	protected:
		uint32_t m_Framebuffer;
		uint32_t m_Renderbuffer;

		uint32_t m_Tex;

		uint32_t m_Width;
		uint32_t m_Height;
	};//GBuffer

}//name space


#endif 