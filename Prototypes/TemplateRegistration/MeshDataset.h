#ifndef __TEMPREG_MESHDATASET_H__
#define __TEMPREG_MESHDATASET_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

#include <Eigen/Eigen>

using namespace Eigen;

namespace TempReg {
	
	enum class DatasetType : int8_t {
		NONE = -1,
		TEMPLATE = 0,		// original template
		DTEMPLATE = 1,		// deformed template
		TARGET = 2			// target
	};

	enum class DatasetVisualizationMode : int8_t {
		SOLID_COLOR = 0,
		HAUSDORFF_DISTANCE = 1
		//... WIREFRAME?
	};

	class MeshDataset {
	public:
		MeshDataset(void);
		~MeshDataset();

		void initFromFile(std::string Filepath, bool ComputeNormals, Vector3f DefaultColor);
		void initFromMeshDataset(MeshDataset& Dataset); //TODO

		void visualizationMode(DatasetVisualizationMode Mode);

		const CForge::T3DMesh<float>* mesh(void);
		
	private:
		CForge::T3DMesh<float> m_RawMesh;
		// TODO Mesh BVH (MeshOctree)
		// TODO Half-Edge datastructure
		// TODO mathematical representation for registration process? (e.g. vertex positions stacked into matrix, ...)

		DatasetVisualizationMode m_VisualizationMode;
	};
}

#endif