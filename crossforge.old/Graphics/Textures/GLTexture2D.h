/*****************************************************************************\
*                                                                           *
* File(s): GLTexture.h and GLTexture.cpp                                     *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_GLTEXTURE2D_H__
#define __CFORGE_GLTEXTURE2D_H__

#include "../../Core/CForgeObject.h"
#include "../../AssetIO/T2DImage.hpp"

namespace CForge {
	/**
	* \brief OpenGL 2D texture object for common raster images.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API GLTexture2D: public CForgeObject {
	public:
		GLTexture2D(void);
        GLTexture2D(uint32_t handle);
		~GLTexture2D(void);

		void init(const T2DImage<uint8_t>* pImage, bool GenerateMipmaps = false);
		void clear(void);

		void bind(void);
		void unbind(void);
		uint32_t handle(void)const;

		int32_t width(void)const;
		int32_t height(void)const;
	private:
		uint32_t m_TexObj;
		int32_t m_Width;
		int32_t m_Height;
	};//GLTexture2D
}//name space


#endif 