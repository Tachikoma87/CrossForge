#pragma once

#include "../../../CForge/Core/CForgeObject.h"
#include "CForge/AssetIO/T3DMesh.hpp"

using namespace Eigen;

namespace CForge {
	
	class CFORGE_API InstanceVertexUtility : public CForgeObject {
	public:
		enum VertexProperty :uint16_t {
			VPROP_POSITION = 0x0001,
			VPROP_NORMAL = 0x0002,
			VPROP_TANGENT = 0x0004,
			VPROP_UVW = 0x0008,
		};

		InstanceVertexUtility(void);
		~InstanceVertexUtility(void);

		void init(uint16_t Properties);
		void clear(void);

		bool hasProperties(VertexProperty Prop);

		uint16_t offset(VertexProperty Prop);
		uint16_t vertexSize(void)const;

		void buildBuffer(uint32_t VertexCount, void** ppBuffer, uint32_t* pBufferSize, const T3DMesh<float>* pMesh, std::vector<Matrix4f>& transMatrixes);

	private:
		uint16_t m_PositionOffset;
		uint16_t m_NormalOffset;
		uint16_t m_TangentOffset;
		uint16_t m_UVWOffset;

		uint16_t m_InstanceMatrixesOffset;

		uint16_t m_Properties;
		uint16_t m_VertexSize;
	};
}
