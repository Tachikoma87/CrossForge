/*****************************************************************************\
*                                                                           *
* File(s): GLVertexArray.h and GLVertexArray.cpp                            *
*                                                                           *
* Content: OpenGL vertex array object that is required by core context to   *
*          render objects.                                                  *
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
#ifndef __CFORGE_GLVERTEXARRAY_H__
#define __CFORGE_GLVERTEXARRAY_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	/**
	* \brief OpenGL vertex array object that is required by core context to render objects.
	* \ingroup Graphics
	*/
	class CFORGE_API GLVertexArray: public CForgeObject {
	public:
		/**
		* \brief Static method that checks whether vertex arrays are available.
		* 
		* \return Whether or not vertex arrays are available on the system.
		*/
		static bool available(void);

		/**
		* \brief Constructor
		*/
		GLVertexArray(void);

		/**
		* \brief Destructor
		*/
		~GLVertexArray(void);

		/**
		* \brief Initialization method. Requires a valid OpenGL context present.
		*/
		void init(void);

		/**
		* \brief Clear method.
		*/
		void clear(void);
		
		/**
		* \brief Bind the vertex array object.
		*/
		void bind(void);

		/**
		* \brief Unbind the vertex array object.
		*/
		void unbind(void);

	protected:
		
	private:
		uint32_t m_GLID;	///< OpenGL identifier of the vertex array.
	};//GLVertexArray

}//name space


#endif