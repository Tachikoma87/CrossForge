/*****************************************************************************\
*                                                                           *
* File(s): UniformBufferComponent.h and UniformBufferComponent.cpp                       *
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
#ifndef __CROSSFORGE_UNIFORMBUFFERCOMPONENT_H__
#define __CROSSFORGE_UNIFORMBUFFERCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class UniformBufferComponent : public ComponentBase {
	public:
		static inline std::string identification = "UniformBufferComponent";

		
		uint32_t getGlBufferHandle();
		uint32_t getBufferSize();

		~UniformBufferComponent();
	protected:
		UniformBufferComponent(const std::string childIdentification);

		bool initialize(uint32_t bufferSize);
		void clear();
		void setSubData(const void* pData, uint32_t dataSize, uint32_t bufferOffset);

		uint32_t m_glBufferHandle;
		uint32_t m_bufferSize;

	};

	typedef std::shared_ptr<UniformBufferComponent> UniformBufferComponentPtr;
}

#endif 