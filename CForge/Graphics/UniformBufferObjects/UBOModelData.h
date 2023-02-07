/*****************************************************************************\
*                                                                           *
* File(s): UBOModelData.h and UBOModelData.cpp                       *
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
#ifndef __CFORGE_UBOMODELDATA_H__
#define __CFORGE_UBOMODELDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for model related data.
	* 
	* \todo Do full documentation.
	*/
	class CFORGE_API UBOModelData: public CForgeObject {
	public:
		UBOModelData(void);
		~UBOModelData(void);

		void init(void);
		void clear(void);
		void bind(uint32_t BindingPoint);
		uint32_t size(void)const;

		void modelMatrix(Eigen::Matrix4f Mat);
		void normalMatrix(Eigen::Matrix4f Mat);

	protected:
		GLBuffer m_Buffer;
		uint32_t m_ModelMatrixOffset;
		uint32_t m_NormalMatrixOffset;
	};//UBOModelData


}//name space

#endif