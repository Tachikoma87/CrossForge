/*****************************************************************************\
*                                                                           *
* File(s): ViewFrustum.h and ViewFrustum.cpp                                *
*                                                                           *
* Content:                    *
*                *
*                               *
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
#ifndef __CFORGE_VIEWFRUSTUM_H__
#define __CFORGE_VIEWFRUSTUM_H__

#include "../../Core/CForgeObject.h"
#include "../../Math/Plane.hpp"
#include "../../Math/BoundingVolume.h"


namespace CForge {
	class CFORGE_API ViewFrustum: public CForgeObject {
	public:
		enum Planes : int8_t {
			PLANE_TOP = 0,
			PLANE_BOTTOM,
			PLANE_RIGHT,
			PLANE_LEFT,
			PLANE_FAR,
			PLANE_NEAR,
			PLANE_COUNT,
		};

		ViewFrustum(void);
		~ViewFrustum(void);

		void init(class VirtualCamera* pCamera);
		void clear(void);

		// thanks to: https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
		void update(void);

		bool visible(const BoundingVolume BV, const Eigen::Quaternionf Rot, const Eigen::Vector3f Trans, const Eigen::Vector3f Scale)const;

		bool visible(const Sphere BS, const Eigen::Quaternionf Rot, const Eigen::Vector3f Trans, const Eigen::Vector3f Scale)const;

		// thanks to: https://www.braynzarsoft.net/viewtutorial/q16390-34-aabb-cpu-side-frustum-culling
		bool visible(const Box AABB, const Eigen::Quaternionf Rot, const Eigen::Vector3f Trans, const Eigen::Vector3f Scale)const;

	protected:
		Plane m_Planes[PLANE_COUNT];
		class VirtualCamera* m_pCamera;

	};//ViewFrustum

}//name space

#endif 