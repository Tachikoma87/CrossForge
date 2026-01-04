/*****************************************************************************\
*                                                                           *
* File(s): VertexBufferComponent.h and VertexBufferComponent.cpp                       *
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
#ifndef __CROSSFORGE_VERTEXBUFFERCOMPONENT_H__
#define __CROSSFORGE_VERTEXBUFFERCOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class VertexBufferComponent : public ComponentBase {
	public:
		static inline std::string identification = "VertexBufferComponent";

		enum VertexAttribute: uint16_t {
			ATTRIBUTE_POSITION	= 0x0001,
			ATTRIBUTE_NORMAL	= 0x0002,
			ATTRIBUTE_TANGENT	= 0x0004,
			ATTRIBUTE_UVW		= 0x0008,
			ATTRIBUTE_COLOR		= 0x0010,
			ATTRIBUTE_BONE_WEIGHTS = 0x0020,
			ATTRIBUTE_BONE_INDICES = 0x0040
		};

		VertexBufferComponent();
		~VertexBufferComponent();

		void initialize(const std::shared_ptr<const VertexBufferComponent> pRef = nullptr);
		void clear() override;

		const bool hasAttribute(VertexAttribute attrib)const;
		void addAttribute(VertexAttribute attrib);
		void removeAttribute(VertexAttribute attrib);

		uint16_t& attributeMask();
		uint32_t& glBufferHandle();
		uint32_t& attributeOffset(VertexAttribute attrib);
		uint32_t& vertexSize();
		uint32_t& vertexCount();

		const uint16_t getAttributeMask()const;
		const uint32_t getGlBufferHandle()const;
		const uint32_t getAttributeOffset(const VertexAttribute attrib)const;
		const uint32_t getVertexSize()const;
		const uint32_t getVertexCount()const;

		void setAttributeMask(const uint16_t attributeMask);
		void setGlBufferHandle(const uint32_t handle);
		void setAttributeOffset(const uint32_t offset, VertexAttribute attrib);
		void setVertexSize(const uint32_t vertexSize);
		void setVertexCount(const uint32_t vertexCount);

	protected:
		VertexBufferComponent(const std::string childIdentification);

		const int8_t getIndexFromAttribute(VertexAttribute attrib)const;

		uint32_t m_glBufferHandle;
		uint16_t m_attributeMask;
		uint32_t m_attributeOffsets[8];
		uint32_t m_vertexSize;
		uint32_t m_vertexCount;
	};

	using VertexBufferComponentPtr = std::shared_ptr<VertexBufferComponent>;
	using VertexBufferComponentCPtr = std::shared_ptr<const VertexBufferComponent>;
}

#endif 