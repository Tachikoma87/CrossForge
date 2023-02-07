/*****************************************************************************\
*                                                                           *
* File(s): VertexUtility.h and VertexUtility.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_VERTEXUTILITY_H__
#define __CFORGE_VERTEXUTILITY_H__

#include "../../Core/CForgeObject.h"
#include "../../AssetIO/T3DMesh.hpp"

namespace CForge {
	/**
	* \brief Utility class that builds the vertex buffer according to the mesh's properties.
	*
	* \todo Do full documentation.
	*
	*/
	class CFORGE_API VertexUtility: public CForgeObject {
	public:
		enum VertexProperty:uint64_t {
			VPROP_POSITION			= 0x0001,
			VPROP_NORMAL			= 0x0002,
			VPROP_TANGENT			= 0x0004,
			VPROP_UVW				= 0x0008,
			VPROP_COLOR				= 0x0010, ///< vertex colors
			VPROP_BONEINDICES		= 0x0020, ///< bone indices
			VPROP_BONEWEIGHTS		= 0x0040, ///< bone weights
		};

		VertexUtility(void);
		~VertexUtility(void);

		void init(uint16_t Properties);
		void clear(void);

		bool hasProperties(VertexProperty Prop);

		uint16_t offset(VertexProperty Prop);
		uint16_t vertexSize(void)const;

		void buildBuffer(uint32_t VertexCount, void** ppBuffer, uint32_t* pBufferSize, const T3DMesh<float>* pMesh = nullptr );

	protected:

	private:
		uint16_t m_PositionOffset;
		uint16_t m_NormalOffset;
		uint16_t m_TangentOffset;
		uint16_t m_UVWOffset;
		uint16_t m_ColorOffset;
		
		uint16_t m_BoneIndicesOffset;
		uint16_t m_BoneWeightsOffset;

		uint16_t m_Properties;
		uint16_t m_VertexSize;

	};//VertexUtility

}//CForge

#endif