/*****************************************************************************\
*                                                                           *
* File(s): UBOBoneData.h and UBOBoneData.cpp                                   *
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
#ifndef __CFORGE_UBOBONEDATA_H__
#define __CFORGE_UBOBONEDATA_H__

#include "../../CForge/Graphics/GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object skeletal animation (Bones) related data.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT UBOBoneData : public CForgeObject {
	public:
		UBOBoneData(void);
		~UBOBoneData(void);

		void init(uint32_t BoneCount);
		void clear(void);

		void bind(uint32_t BindingPoint);

		void skinningMatrix(uint32_t Index, Eigen::Matrix4f SkinningMat);

		uint32_t size(void)const;

	protected:

	private:
		GLBuffer m_Buffer;
		uint32_t m_BoneCount;
	};//UBOBoneData

}//name space

#endif