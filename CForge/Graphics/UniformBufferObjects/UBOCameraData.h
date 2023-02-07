/*****************************************************************************\
*                                                                           *
* File(s): UBOCameraData.h and UBOCameraData.cpp                                    *
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
#ifndef __CFORGE_UBOCAMERADATA_H__
#define __CFORGE_UBOCAMERADATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for camera related data.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API UBOCameraData: public CForgeObject {
	public:
		UBOCameraData(void);
		~UBOCameraData(void);

		void init(void);
		void clear(void);

		void bind(uint32_t BindingPoint);

		void viewMatrix(const Eigen::Matrix4f Mat);
		void projectionMatrix(const Eigen::Matrix4f Mat);
		void position(const Eigen::Vector3f Pos);
		uint32_t size(void)const;

	protected:
		
	private:
		uint32_t m_ViewMatrixOffset;
		uint32_t m_ProjectionMatrixOffset;
		uint32_t m_Positionoffset;

		GLBuffer m_Buffer;

	};//UBOCameraData

}//name space

#endif