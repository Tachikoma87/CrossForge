/*****************************************************************************\
*                                                                           *
* File(s): IRenderableMesh.h and IRenderableMesh.cpp                        *
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
#ifndef __CFORGE_IRENDERABLEACTOR_H__
#define __CFORGE_IRENDERABLEACTOR_H__

#include "../../Core/CForgeObject.h"
#include "VertexUtility.h"
#include "RenderGroupUtility.h"
#include "../GLBuffer.h"
#include "../GLVertexArray.h"
#include "../../Math/BoundingVolume.h"

namespace CForge {

	/**
	* \brief Basic interface for an actor that can be drawn to the screen.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API IRenderableActor: public CForgeObject {
	public:
		enum ActorType: int32_t {
			ATYPE_UNKNOWN = -1,
			ATYPE_STATIC = 0,
			ATYPE_SKELETAL = 1,
			ATYPE_SCREENQUAD = 2,
		};

		virtual void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) = 0;
		
		int32_t typeID(void)const;

		uint32_t materialCount(void)const;
		RenderMaterial* material(uint32_t Index);
		const RenderMaterial* material(uint32_t Index)const;

		virtual BoundingVolume boundingVolume(void)const;
		virtual void boundingVolume(const BoundingVolume BV);

		virtual Box getAABB();
		
	protected:
		IRenderableActor(const std::string ClassName, int32_t ActorType);
		virtual ~IRenderableActor(void);

		virtual void setBufferData(void);

		GLBuffer m_VertexBuffer; ///< stores vertex data
		GLBuffer m_ElementBuffer; ///< stores triangle indexes
		GLVertexArray m_VertexArray; ///< the vertex array

		VertexUtility m_VertexUtility;
		RenderGroupUtility m_RenderGroupUtility;

		int32_t m_TypeID;
		std::string m_TypeName;

		BoundingVolume m_BV;
		
	private:
		
	};//IRenderableActor
}//name space

#endif