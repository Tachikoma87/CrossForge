/*****************************************************************************\
*                                                                           *
* File(s): TriangleMeshController.h and TriangleMeshController.cpp                     *
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
#ifndef __CROSSFORGE_TRIANGLEMESHCONTROLLER_H__
#define __CROSSFORGE_TRIANGLEMESHCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include <crossforge/assetio/entities/TriangleMeshEntity.h>

namespace crossforge {
	class TriangleMeshController : public ControllerBase {
	public:
		static inline std::string identification = "TriangleMeshController";

		static bool recomputeVertexNormals(TriangleMeshEntityPtr pTriangleMesh);
		static bool recomputeVertexTangents(TriangleMeshEntityPtr pTriangleMesh);

		/**
		* \brief A two dimensional plane.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and height of the object.
		* \param[in] Segments Number of segments in both directions.
		*/
		static void plane(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f dimensions, Eigen::Vector2i segments, bool twoSided = false);

		/**
		* \brief A circle
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and height of the object.
		* \param[in] Slices Number of slices . More slices mean higher tessellation and a less coarse object.
		* \param[in] TipOffset The central point will be moved out of the plane by this amount, resulting in a cone like shape.
		*/
		static void circle(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f dimensions, uint32_t slices, float tipOffset, bool twoSided = false);

		/**
		* \brief Creates a box.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, height, and depth of the object.
		* \param[in] Segments Number of segments across the x,y, and z plane.
		*/
		static void cuboid(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimenisions, Eigen::Vector3i segments);

		/**
		* \brief A sphere with uv tessellation.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, height, and depth of the object.
		* \param[in] Slices Tessellation in the xz plane.
		* \param[in] Stacks Tessellation in the yx plane.
		*/
		static void uvSphere(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimensions, uint32_t slices, uint32_t stacks);

		/**
		* \brief A cone.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width, depth, and height of the object.
		* \param[in] Slices Tessellation.
		*/
		static void cone(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector3f dimensions, uint32_t slices);

		/**
		* \brief Two cones attached on the round bottom forming a diamond like shape.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Dimensions Width and depth of the object as well as offset values for the two tips.
		* \param[in] Slices Tessellation.
		*/
		static void doubleCone(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector4f dimensions, uint32_t slices);

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
		static void cylinder(TriangleMeshEntityPtr pTriangleMesh, Eigen::Vector2f topDimensions, Eigen::Vector2f bottomDimensions, float height, uint32_t slices, Eigen::Vector2f tipOffsets);

		/**
		* \brief A donut like shape.
		*
		* \param[out] pMesh Mesh data structure where the resulting mesh will be stored.
		* \param[in] Radius Outer radius.
		* \param[in] Thickness Thickness of the torus.
		* \param[in] Slices Tessellation in the horizontal plane.
		* \param[in] Stacks Tessellation in the vertical plane.
		*/
		static void torus(TriangleMeshEntityPtr pTriangleMesh, float radius, float thickness, uint32_t slices, uint32_t stacks);

		~TriangleMeshController();
	protected:
		TriangleMeshController(const std::string childIdentification);
	};

	typedef std::shared_ptr<TriangleMeshController> TriangleMeshControllerPtr;
}

#endif 