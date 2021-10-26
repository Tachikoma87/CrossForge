/*****************************************************************************\
*                                                                           *
* File(s): GLVertexArray.h and GLVertexArray.cpp                             *
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
#ifndef __CFORGE_GLVERTEXARRAY_H__
#define __CFORGE_GLVERTEXARRAY_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	/**
	* \brief OpenGL vertex array object that is required by core context to render objects.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT GLVertexArray: public CForgeObject {
	public:
		static bool available(void);

		GLVertexArray(void);
		~GLVertexArray(void);

		void init(void);
		void clear(void);
		
		void bind(void);
		void unbind(void);

	protected:
		
	private:
		uint32_t m_GLID;
	};//GLVertexArray

}//name space


#endif