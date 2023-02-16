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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_BOUNDINGVOLUME_H__
#define __CFORGE_BOUNDINGVOLUME_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T3DMesh.hpp"
#include "Box.hpp"
#include "Sphere.hpp"

namespace CForge {
	class CFORGE_API BoundingVolume: public CForgeObject {
	public:
		enum Type : int8_t {
			TYPE_UNKNOWN = -1,
			TYPE_AABB,
			TYPE_SPHERE,
		};

		BoundingVolume(void);
		~BoundingVolume(void);

		void init(const T3DMesh<float>* pMesh, Type T);
		void init(const Box AABB);
		void init(const Sphere BS);
		void clear(void);
		void release(void);

		Box aabb(void)const;
		Sphere boundingSphere(void)const;
		Type type(void)const;

	protected:
		Sphere m_Sphere;
		Box m_AABB;
		Type m_Type; ///< The main type

	};//BoundingVolume


}//name space

#endif 