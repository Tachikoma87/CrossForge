/*****************************************************************************\
*                                                                           *
* File(s): SurfaceSampler.h and SurfaceSampler.cpp                                             *
*                                                                           *
* Content:    *
*                                                   *
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
#ifndef __CFORGE_SURFACESAMPLER_H__
#define __CFORGE_SURFACESAMPLER_H__

#include <crossforge/Core/CForgeObject.h>
#include <crossforge/AssetIO/SAssetIO.h>

namespace CForge {
	class SurfaceSampler : public CForgeObject {
	public:
		SurfaceSampler(void);
		~SurfaceSampler(void);

		void init(T3DMesh<float> *pMesh);
		void clear(void);
		void release(void);

		void sampleEquidistant(float MaxSampleDistance, int32_t MaxSamplePoints, std::vector<int32_t>* pSamplePoints);

	private:
		T3DMesh<float>* m_pMesh;		///< The mesh to process.
		std::vector<uint32_t> m_Nodes;	///< The sampled nodes.

		void toIGLMesh(Eigen::MatrixXf *pVertices, Eigen::MatrixXi *pFaces, const T3DMesh<float>* pMesh);

		Eigen::MatrixXf m_Vertices; // Vertex data
		Eigen::MatrixXi m_Faces; // Face data

	};//SurfaceSampler


}//name space


#endif 