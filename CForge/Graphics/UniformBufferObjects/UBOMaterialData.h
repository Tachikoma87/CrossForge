/*****************************************************************************\
*                                                                           *
* File(s): UBOMaterialData.h and UBOMaterialData.cpp                        *
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
#ifndef __CFORGE_UBOMATERIALDATA_H__
#define __CFORGE_UBOMATERIALDATA_H__


#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for material related data.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT UBOMaterialData: public CForgeObject {
	public:
		UBOMaterialData(void);
		~UBOMaterialData(void);

		void init(void);
		void clear(void);
		void bind(uint32_t BindingPoint);
		uint32_t size(void)const;

		void color(Eigen::Vector4f Color);
		void metallic(float Metallicness);
		void roughness(float Roughness);
		void ambientOcclusion(float AO);

	protected:
		uint32_t m_ColorOffset;
		uint32_t m_MetallicOffset;
		uint32_t m_RoughnessOffset;
		uint32_t m_AmbientOcclusionOffset;

		GLBuffer m_Buffer;
	};//UBOMaterialData

}//name space


#endif