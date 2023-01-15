/*****************************************************************************\
*                                                                           *
* File(s): BoundingVolume.hpp                               *
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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_BOUNDINGVOLUME_H__
#define __CFORGE_BOUNDINGVOLUME_H__

#include <CForge/Core/CForgeObject.h>
#include <CForge/AssetIO/T3DMesh.hpp>
#include "BoundingSphere.hpp"
#include "AABB.hpp"

namespace CForge {
	class BoundingVolume {
	public:
		enum Type: int8_t {
			TYPE_UNKNOWN = -1,
			TYPE_AABB,
			TYPE_SPHERE,
		};

		BoundingVolume(void);
		~BoundingVolume(void);

		void init(const T3DMesh<float>* pMesh, Type T);
		void init(const AABB Box);
		void init(const BoundingSphere Sphere);
		void clear(void);
		void release(void);

		AABB aabb(void)const;
		BoundingSphere boundingSphere(void)const;
		Type type(void)const;

	protected:
		BoundingSphere m_Sphere;
		AABB m_AABB;
		Type m_Type; ///< The main type

	};//BoundingVolume


}//name space

#endif 