/*****************************************************************************\
*                                                                           *
* File(s): PrimitiveFactory.h and PrimitiveFactory.cpp                      *
*                                                                           *
* Content: Creates various basic shapes and objects in a programmatic way.  *
*                                                                           *
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
#ifndef __CFORGE_PRIMITIVESHAPEFACTORY_H__
#define __CFORGE_PRIMITIVESHAPEFACTORY_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T3DMesh.hpp"

namespace CForge {
	/**
	* \brief Creates various basic shapes and objects in a programmatic way.
	* \ingroup MeshProcessing
	* 
	* The generated meshes will also have uv coordinates. See the ShapesAndMaterials example for examples what this class can do.
	* \todo Change pass by pointer to pass by reference.
	*/
	class CFORGE_API PrimitiveShapeFactory : public CForgeObject {
	public:
		/**
		* \brief A two dimensional plane.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and height of the object.
		* \param[in] Segments Number of segments in both directions.
		*/
		static void plane(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, Eigen::Vector2i Segments);

		/**
		* \brief A circle
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and height of the object.
		* \param[in] Slices Number of slices . More slices mean higher tessellation and a less coarse object.
		* \param[in] TipOffset The central point will be moved out of the plane by this amount, resulting in a cone like shape.
		*/
		static void circle(T3DMesh<float>* pMesh, Eigen::Vector2f Dimensions, uint32_t Slices, float TipOffset);

		/**
		* \brief Creates a box.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, height, and depth of the object.
		* \param[in] Segments Number of segments across the x,y, and z plane.
		*/
		static void cuboid(T3DMesh<float>* pMesh, Eigen::Vector3f Dimenisions, Eigen::Vector3i Segments);

		/**
		* \brief A sphere with uv tessellation.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, height, and depth of the object.
		* \param[in] Slices Tessellation in the xz plane.
		* \param[in] Stacks Tessellation in the yx plane.
		*/
		static void uvSphere(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices, uint32_t Stacks);

		/**
		* \brief A cone.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, depth, and height of the object.
		* \param[in] Slices Tessellation.
		*/
		static void cone(T3DMesh<float>* pMesh, Eigen::Vector3f Dimensions, uint32_t Slices);

		/**
		* \brief Two cones attached on the round bottom forming a diamond like shape.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and depth of the object as well as offset values for the two tips.
		* \param[in] Slices Tessellation.
		*/
		static void doubleCone(T3DMesh<float>* pMesh, Eigen::Vector4f Dimensions, uint32_t Slices);

		/**
		* \brief A cylindrical shape.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] TopDimension Width and depth of the top circle.
		* \param[in] BottomDimension Width and depth of the bottom circle.
		* \param[in] Height Height of the object.
		* \param[in] Slices Tessellation.
		* \param[in] TipOffset Offset values for the top and bottom tip vertexes.
		*/
		static void cylinder(T3DMesh<float>* pMesh, Eigen::Vector2f TopDimensions, Eigen::Vector2f BottomDimensions, float Height, uint32_t Slices, Eigen::Vector2f TipOffsets);

		/**
		* \brief A donut like shape.
		* 
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Radius Outer radius.
		* \param[in] Thickness Thickness of the torus.
		* \param[in] Slices Tessellation in the horizontal plane.
		* \param[in] Stacks Tessellation in the vertical plane.
		*/
		static void torus(T3DMesh<float>* pMesh, float Radius, float Thickness, uint32_t Slices, uint32_t Stacks);

		/**
		* \brief Constructor
		*/
		PrimitiveShapeFactory(void);

		/**
		* \brief Destructor
		*/
		~PrimitiveShapeFactory(void);

	protected:

	};//PrimitiveShapeFactory
}


#endif 