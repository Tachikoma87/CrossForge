/*****************************************************************************\
*                                                                           *
* File(s): GLCubemap.h and GLCubemap.cpp                                    *
*                                                                           *
* Content: This class creates and manages a cube map OpenGL object.         *
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
#ifndef __CFORGE_GLCUBEMAPS_H__
#define __CFORGE_GLCUBEMAPS_H__

#include "../../Core/CForgeObject.h"
#include "../../AssetIO/T2DImage.hpp"

namespace CForge {
	/**
	* \brief This class creates and manages a cube map OpenGL object.
	* \ingroup Textures
	* 
	* \todo Change pass by pointer to pass by reference.
	*/
	class CFORGE_API GLCubemap: public CForgeObject {
	public:
		/**
		* \brief Constructor.
		*/
		GLCubemap(void);

		/**
		* \brief Destructor.
		*/
		~GLCubemap(void);

		/**
		* \brief Initialization method. It takes six images for each plane of the cube and uses them as texture data.
		* 
		* \param[in] pRight Image for the right plane.
		* \param[in] pLeft Image for the left plane.
		* \param[in] pTop Image for the top plane.
		* \param[in] pBottom Image for the bottom plane.
		* \param[in] pBack Image for the back plane.
		* \param[in] pFront Image for the front plane.
		*/
		void init(	const T2DImage<uint8_t>* pRight, const T2DImage<uint8_t>* pLeft, const T2DImage<uint8_t>* pTop, 
					const T2DImage<uint8_t>* pBottom, T2DImage<uint8_t>* pBack, T2DImage<uint8_t>* pFront);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the cube map.
		*/
		void bind(void);

		/**
		* \brief Unbinds the cube map.
		*/
		void unbind(void);

		/**
		* \brief Returns the OpenGL handle.
		* 
		* \return OpenGL cube map handle.
		* \warning Be careful what you do with it!
		*/
		uint32_t handle(void)const;

	protected:
		uint32_t m_TexObj;		///< OpenGL object handle.

	};//GLCubemap

}//name space

#endif 