/*****************************************************************************\
*                                                                           *
* File(s): IndexBufferComponent.h and IndexBufferComponent.cpp                      *
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
#ifndef __CROSSFORGE_INDEXBUFFERCOMPONENT_H__
#define __CROSSFORGE_INDEXBUFFERCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class IndexBufferComponent : public ComponentBase {
	public:
		static inline std::string identification = "IndexBufferComponent";

		IndexBufferComponent();
		~IndexBufferComponent();


		uint32_t& glBufferHandle();
		uint64_t& indexCount();
		uint64_t& bufferSize();

	protected:
		IndexBufferComponent(const std::string childIdentification);

		uint32_t m_glBufferHandle;
		uint64_t m_indexCount;
		uint64_t m_bufferSize;
	};

	typedef std::shared_ptr<IndexBufferComponent> IndexBufferComponentPtr;
}

#endif