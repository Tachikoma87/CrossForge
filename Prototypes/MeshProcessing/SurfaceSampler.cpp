#include "SurfaceSampler.h"

#include <igl/exact_geodesic.h>
#include <igl/harmonic.h>
#include <igl/writeOBJ.h>

//#include <iostream>
//#include <fstream>

using namespace Eigen;

namespace CForge {

	SurfaceSampler::SurfaceSampler(void): CForgeObject("SurfaceSampler") {

	}//Constructor

	SurfaceSampler::~SurfaceSampler(void) {

	}//Destructor

	void SurfaceSampler::init(T3DMesh<float> *pMesh) {
		clear();
		toIGLMesh(&m_Vertices, &m_Faces, pMesh);
		//m_pMesh = pMesh;
	}//initialize

	void SurfaceSampler::clear(void) {
		m_pMesh = nullptr;
		m_Nodes.clear();
	}//clear

	void SurfaceSampler::release(void) {
		delete this;
	}//release
	/*
	bool SurfaceSampler::sameVertex(float distance, int32_t vert1, int32_t vert2) {
		Eigen::Vector3f pVertex1(m_Vertices.row(vert1));
		Eigen::Vector3f pVertex2(m_Vertices.row(vert2));

		Eigen::Vector3f dist = pVertex1 - pVertex2;
		float actualDistance = sqrt(dist.x() * dist.x() + dist.y() * dist.y() + dist.z() * dist.z());
		if (actualDistance < distance)
			return true;
		return false;

	}

	void SurfaceSampler::removeDoubleVerts() {

		vertexMapping.resize(m_Vertices.rows());
		for (int32_t i = 0; i < vertexMapping.rows(); i++) {
			vertexMapping(i) = -1;
		}
		int32_t accurateIndex = 0;

		for (int32_t i = 0; i < m_Vertices.rows(); i++) {
			
			if (vertexMapping(i) != -1)
				continue;

			vertexMapping(i) = accurateIndex;

			for (int j = i+1; j < m_Vertices.rows(); j++) {

				bool sameVerts = sameVertex(0.00001f, i, j);

				if (sameVerts == true) {
					vertexMapping(j) = vertexMapping(i);
				}

			}
			accurateIndex += 1;
		}



		std::ofstream myfile("Assets/tmp/vertexMapping.txt");
		if (myfile.is_open())
		{
			myfile << vertexMapping.size() << "\n";

			for (int i = 0; i < vertexMapping.size(); i++)
			{
				myfile << vertexMapping(i) << "\n";
			}
			myfile.close();
		}
		else std::cout << "Unable to open file";

	}

	void SurfaceSampler::editMatrices(void) {
		MatrixXf newV(m_Vertices.rows(), m_Vertices.cols());
		MatrixXi newF(m_Faces.rows(), m_Faces.cols());

		int32_t accurateIndex = 0;
		for (int i = 0; i < m_Vertices.rows(); i++) {
			if (vertexMapping(i) == accurateIndex) {
				newV.row(accurateIndex) = m_Vertices.row(i);
				accurateIndex += 1;
			}
		}

		newV.resize(accurateIndex, m_Vertices.cols());

		for (int i = 0; i < m_Faces.rows(); i++) {
			int32_t vert1 = vertexMapping(m_Faces(i, 0));
			int32_t vert2 = vertexMapping(m_Faces(i, 1));
			int32_t vert3 = vertexMapping(m_Faces(i, 2));
			newF.row(i) = Eigen::RowVector3i(vert1, vert2, vert3);
		}
		this->m_Vertices = newV;
		this->m_Faces = newF;

	}
	*/
	void SurfaceSampler::sampleEquidistant(float MaxSampleDistance, int32_t MaxSamplePoints, std::vector<int32_t> *pSamplePoints) {

		int32_t NextSample = 0;
	
		Eigen::VectorXi VS, FS, VT, FT;
		Eigen::VectorXf d;
		

		if(pSamplePoints->size() == 0) pSamplePoints->push_back(CoreUtility::rand() % m_Vertices.rows());

		// set targets (all vertices)
		VT.setLinSpaced(m_Vertices.rows(), 0, m_Vertices.rows() - 1);
		do {
			// set sources
			VS.resize(pSamplePoints->size());
			for (uint32_t i = 0; i < pSamplePoints->size(); ++i) VS[i] = pSamplePoints->at(i);

			// compute exact geodesics
			igl::exact_geodesic(m_Vertices, m_Faces, VS, FS, VT, FT, d);

			for (int32_t i = 0; i < d.rows(); ++i) {
				if (d[i] > d[NextSample]) NextSample = i;
			}

			pSamplePoints->push_back(NextSample);

			printf("Samples: %d | Maximum geodesic: %.2f | NextSample: %d\n", (int32_t)pSamplePoints->size(), d[NextSample], NextSample);

		} while (d[NextSample] > MaxSampleDistance && pSamplePoints->size() < MaxSamplePoints);//while[not enough samples]
		/*
		std::sort(pSamplePoints->data(), pSamplePoints->data() + pSamplePoints->size());
		U_bc.resize(pSamplePoints->size(), m_Vertices.cols());
		V_bc.resize(pSamplePoints->size(), m_Vertices.cols());
		b.resize(pSamplePoints->size());
		
		for (int bi = 0; bi < pSamplePoints->size(); bi++)
		{
			b(bi) = pSamplePoints->at(bi);
			V_bc.row(bi) = m_Vertices.row(b(bi));
			U_bc.row(bi) = m_Vertices.row(b(bi));
		}

		U_bc.row(5) = m_Vertices.row(b(5)) + RowVector3f(0, 0, 10);
		
		const MatrixXf U_bc_anim = U_bc;//V_bc + bc_frac * (U_bc - V_bc);
		MatrixXf D_bc = U_bc_anim - V_bc;
		igl::harmonic(m_Vertices, m_Faces, b, D_bc, 2, D);
		m_Vertices = m_Vertices + D;
		igl::writeOBJ("Test.obj", m_Vertices, m_Faces);*/

		
		std::sort(pSamplePoints->data(), pSamplePoints->data() + pSamplePoints->size());
		std::ofstream myfile("Assets/tmp/EveSamplePoints1000.txt");
		if (myfile.is_open())
		{
			myfile << pSamplePoints->size() << "\n";

			for (int i = 0; i < pSamplePoints->size(); i++)
			{
				myfile << pSamplePoints->at(i) << "\n";
			}
			myfile.close();
		}
		else std::cout << "Unable to open file";
		

	}//sampleEquidistant

	void SurfaceSampler::toIGLMesh(Eigen::MatrixXf* pVertices, Eigen::MatrixXi* pFaces, const T3DMesh<float>* pMesh) {
		if (nullptr == pVertices) throw NullpointerExcept("pVertices");
		if (nullptr == pFaces) throw NullpointerExcept("pFaces");
		if (nullptr == pMesh) throw NullpointerExcept("pMesh");

		pVertices->resize(pMesh->vertexCount(), 3);
		uint64_t FaceCount = 0;
		for (uint64_t i = 0; i < pMesh->submeshCount(); ++i) FaceCount += pMesh->getSubmesh(i)->Faces.size();
		pFaces->resize(FaceCount, 3);

		// copy vertices
		for (uint64_t i = 0; i < pMesh->vertexCount(); ++i) {
			pVertices->row(i) = pMesh->vertex(i);
		}//for[all vertices]

		// copy faces
		uint64_t FaceIndex = 0;
		for (uint64_t i = 0; i < pMesh->submeshCount(); ++i) {
			auto* pSub = pMesh->getSubmesh(i);

			for (auto k : pSub->Faces) {
				pFaces->row(FaceIndex)[0] = k.Vertices[0];
				pFaces->row(FaceIndex)[1] = k.Vertices[1];
				pFaces->row(FaceIndex)[2] = k.Vertices[2];
				++FaceIndex;
			}//for[all faces]

		}//for[sub meshes]


	}//toIGLMesh


}//names space