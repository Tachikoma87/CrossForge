/*****************************************************************************\
*                                                                           *
* File(s): GLRenderbuffer.h and GLRenderbuffer.cpp                                     *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_GLRENDERBUFFER_H__
#define __CFORGE_GLRENDERBUFFER_H__

#include "../../CForge/Core/CForgeObject.h"
#include "../../CForge/AssetIO/T2DImage.hpp"

namespace CForge {

	class GLRenderbuffer : public CForgeObject {
	public:
		enum Component : int8_t {
			COMP_COLOR = 0,
			COMP_DEPTH_STENCIL
		};

		GLRenderbuffer(void);
		~GLRenderbuffer(void);

		void init(uint32_t Width, uint32_t Height);
		void clear(void);

		void bind(void)const;
		void unbind(void)const;

		void bindTexture(Component Comp, uint32_t Level);
		void blitDepthBuffer(uint32_t Width, uint32_t Height);

		uint32_t width(void)const;
		uint32_t height(void)const;

		void retrieveColorBuffer(T2DImage<uint8_t>* pImg);
		void retrieveDepthBuffer(T2DImage<uint8_t>* pImg, float Near = -1.0f, float Far = -1.0f);
	protected:
		uint32_t m_Framebuffer;
		uint32_t m_Renderbuffer;

		uint32_t m_TexColor;
		uint32_t m_TexDepthStencil;

		uint32_t m_Width;
		uint32_t m_Height;
	};//ForwardBuffer

}//name space

#endif 