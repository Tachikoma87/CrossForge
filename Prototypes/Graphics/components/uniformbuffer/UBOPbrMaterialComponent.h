/*****************************************************************************\
*                                                                           *
* File(s): UBOPBRMaterialComponent.h and UBOPBRMaterialComponent.cpp                       *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_UBOPBRMATERIALCOMPONENT_H__
#define __CROSSFORGE_UBOPBRMATERIALCOMPONENT_H__

#include "UniformBufferComponent.h"

namespace crossforge {
	class UBOPbrMaterialComponent : public UniformBufferComponent {
	public:
		static inline std::string identification = "UBOPBRMaterialComponent";

		UBOPbrMaterialComponent();
		~UBOPbrMaterialComponent();

		void initialize();
		void clear();

		void setColor(const Eigen::Vector4f color);
		void setMetallic(const float metallic);
		void setRougness(const float roughness);
		void setAmbientOcclusion(const float ambientOcclusion);

	protected:
		UBOPbrMaterialComponent(const std::string childIdentification);

		uint32_t m_colorOffset;
		uint32_t m_metallicOffset;
		uint32_t m_roughnessOffset;
		uint32_t m_ambientOcclusionOffset;

	};

	typedef std::shared_ptr<UBOPbrMaterialComponent> UBOPBRMaterialComponentPtr;
}

#endif 