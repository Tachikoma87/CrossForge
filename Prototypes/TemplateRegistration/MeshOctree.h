#pragma once

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {

	struct ElementRef
	{
		int32_t Ref, Next;

		ElementRef() :
			Ref(-1), Next(-1)
		{}

		ElementRef(int32_t Ref_, int32_t Next_) :
			Ref(Ref_), Next(Next_)
		{}
	};

	struct AABB {
		Vector3f Center;
		Vector3f HalfwidthExtents;

		int32_t NextElement; // references first child, first element or nothing (-1) depending on elCount
		uint32_t ElCount;
		uint8_t Depth;

		AABB() :
			Center(Vector3f(0.0f, 0.0f, 0.0f)), HalfwidthExtents(Vector3f(0.0f, 0.0f, 0.0f)), NextElement(-1), ElCount(0), Depth(0)
		{}

		AABB(Vector3f Center_, Vector3f Extents_, int32_t Next_, uint8_t Depth_) :
			Center(Center_), HalfwidthExtents(Extents_), NextElement(Next_), ElCount(0), Depth(Depth_)
		{}
	};

	class MeshOctree {
	public:

	private:

	};
}