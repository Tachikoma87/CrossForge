/*****************************************************************************\
*                                                                           *
* File(s): VertexArrayComponent.h and VertexArrayComponent.cpp                      *
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
#ifndef __CROSSFORGE_VERTEXARRAYCOMPONENT_H__
#define __CROSSFORGE_VERTEXARRAYCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class VertexArrayComponent : public ComponentBase {
	public:
		static inline std::string identification = "VertexArrayComponent";

		VertexArrayComponent();
		~VertexArrayComponent();

		uint32_t& glVertexArrayHandle();

	protected:
		VertexArrayComponent(const std::string childIdentification);

		uint32_t m_glVertexArrayHandle;
	};

	typedef std::shared_ptr<VertexArrayComponent> VertexArrayComponentPtr;
}


#endif 