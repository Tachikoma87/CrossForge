/*****************************************************************************\
*                                                                           *
* File(s): GLCubemap.h and GLCubemap.cpp                                     *
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
#ifndef __CFORGE_GLCUBEMAPS_H__
#define __CFORGE_GLCUBEMAPS_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T2DImage.hpp"

namespace CForge {
	class CFORGE_API GLCubemap : public CForgeObject {
	public:
		GLCubemap(void);
		~GLCubemap(void);

		void init(const T2DImage<uint8_t>* pRight, const T2DImage<uint8_t>* pLeft, const T2DImage<uint8_t>* pTop,
			const T2DImage<uint8_t>* pBottom, T2DImage<uint8_t>* pBack, T2DImage<uint8_t>* pFront);
		void clear(void);
		void release(void);

		void bind(void);
		void unbind(void);
		uint32_t handle(void)const;

	protected:
		uint32_t m_TexObj;

	};//GLCubemap

}//name space

#endif 