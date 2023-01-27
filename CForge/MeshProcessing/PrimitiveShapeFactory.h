/*****************************************************************************\
*                                                                           *
* File(s): PrimitiveFactory.h and PrimitiveFactory.cpp                      *
*                                                                           *
* Content: Creates various basic shapes and objects programmatic.    *
*                                                   *
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
#ifndef __CFORGE_PRIMITIVESHAPEFACTORY_H__
#define __CFORGE_PRIMITIVESHAPEFACTORY_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T3DMesh.hpp"

namespace CForge {
	class CFORGE_API PrimitiveShapeFactory : public CForgeObject {
	public:
		static void plane(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, Eigen::Vector2i Segments);
		static void circle(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, uint32_t Slices, float TipOffset);

		static void cuboid(T3DMesh<float>* pMesh, Eigen::Vector3f Dimenisions, Eigen::Vector3i Segments);
		static void uvSphere(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices, uint32_t Stacks);

		static void cone(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices);
		static void doubleCone(T3DMesh<float>* pMesh, Eigen::Vector4f Dimensions, uint32_t Slices);
		static void cylinder(T3DMesh<float>* pMesh, Eigen::Vector2f TopDimensions, Eigen::Vector2f BottomDimensions, float Height, uint32_t Slices, Eigen::Vector2f TipOffsets);
		static void Torus(T3DMesh<float>* pMesh, float Radius, float Thickness, uint32_t Slices, uint32_t Stacks);

		PrimitiveShapeFactory(void);
		~PrimitiveShapeFactory(void);

	protected:

	};//PrimitiveShapeFactory
}


#endif 