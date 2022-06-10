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

#include "../../CForge/Graphics/GLBuffer.h"

namespace CForge {
	class UBOMorphTargetData {
	public:
		UBOMorphTargetData(void);
		~UBOMorphTargetData(void);

		void init(void);
		void clear(void);

	protected:
		GLBuffer m_Buffer;
	};//UBOMorphTargetData
}//name space

#endif 