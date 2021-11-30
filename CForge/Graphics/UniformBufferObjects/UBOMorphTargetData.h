/*****************************************************************************\
*                                                                           *
* File(s): UBOMorphTargetData.h and UBOMorphTargetData.cpp                    *
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
#ifndef __CFORGE_UBOMORPHTARGETDATA_H__
#define __CFORGE_UBOMORPHTARGETDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	class CFORGE_IXPORT UBOMorphTargetData: public CForgeObject {
	public:
		UBOMorphTargetData(void);
		~UBOMorphTargetData(void);

		void init(void);
		void clear(void);
		void release(void);

		void bind(uint32_t BindingPoint);
		uint32_t size(void)const;

		void setMorphTargetParam(uint32_t Index, uint32_t MorphTargetID, float Strength);
		void activeMorphTargets(int32_t Count);
		void dataOffset(int32_t DataOffset);

	protected:
		GLBuffer m_Buffer;

		uint32_t m_DataOffset;
		uint32_t m_ActivationIDsOffset;
		uint32_t m_ActivationStrengthsOffset;

	};//UBOMorphTargetData
}//name space

#endif 