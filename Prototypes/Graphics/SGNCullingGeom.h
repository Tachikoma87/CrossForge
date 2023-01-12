/*****************************************************************************\
*                                                                           *
* File(s): GLRenderbuffer.h and GLRenderbuffer.cpp                                     *
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
#ifndef __CFORGE_SGNCULLINGGEOM_H__
#define __CFORGE_SGNCULLINGGEOM_H__

#include <CForge/Graphics/SceneGraph/SGNGeometry.h>
#include "../Math/ViewFrustum.h"
#include "../Math/BoundingSphere.hpp"
#include "../Math/AABB.hpp"

namespace CForge {
	class SGNCullingGeom : public SGNGeometry {
	public:
		SGNCullingGeom(void);
		~SGNCullingGeom(void);

		virtual void render(RenderDevice* pRDev, const Eigen::Vector3f Position, const Eigen::Quaternionf Rotation, const Eigen::Vector3f Scale);

		void boundingSphere(BoundingSphere Sphere);
		void aabb(AABB aabb);

		void release(void);

		static uint32_t culled(bool Reset = true);

	protected:
		ViewFrustum m_ViewFrustum;
		BoundingSphere m_BoundingSphere;
		AABB m_AABB;

		static uint32_t m_Culled;
	};

}//name space

#endif 