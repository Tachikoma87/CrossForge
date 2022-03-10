
#include <igl/decimate.h>
#include <igl/read_triangle_mesh.h>
#include <igl/write_triangle_mesh.h>
#include "../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	class MeshDecimator {	

	public:
		static void decimateMesh(CForge::T3DMesh<float>* inMesh, CForge::T3DMesh<float>* outMesh, float amount);

		MeshDecimator(void);
		~MeshDecimator(void);
	private:
	};
}
