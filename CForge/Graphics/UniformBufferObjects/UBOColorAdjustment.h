/*****************************************************************************\
*                                                                           *
* File(s): UBOPostProcessing.h and UBOPostProcessing.cpp                                   *
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
#ifndef __CFORGE_UBOPOSTPROCESSING_H__
#define __CFORGE_UBOPOSTPROCESSING_H__

#include "../GLBuffer.h"

namespace CForge {
	class CFORGE_IXPORT UBOColorAdjustment: public CForgeObject {
	public:
		UBOColorAdjustment(void);
		~UBOColorAdjustment(void);

		void init(void);
		void clear(void);
		void release(void);

		void bind(uint32_t BindingPoint);

		void contrast(float Contrast);
		void saturation(float Saturation);
		void brigthness(float Brightness);

	protected:
		GLBuffer m_Buffer;
	};//UBOColorAdjustment

}//name space

#endif 