#include "MeshDecimate.h"
#include "../CForge/AssetIO/SAssetIO.h"
#include <chrono>

namespace CForge {
	
	// TODO inMesh zu const& machen // cleanup split funcs
	void MeshDecimator::decimateMesh(const CForge::T3DMesh<float>* inMesh, CForge::T3DMesh<float>* outMesh, float amount)
	{
		auto start = std::chrono::high_resolution_clock::now();
		
		uint32_t faceCount = 0;
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			faceCount += inMesh->getSubmesh(i)->Faces.size();
		}
		
		Eigen::MatrixXd DV(inMesh->vertexCount(), 3);
		Eigen::MatrixXi DF(faceCount, 3);
		Eigen::VectorXi DuF; // used faces
		Eigen::VectorXi DuV; // used vertices
		
		for (uint32_t i = 0; i < inMesh->vertexCount(); i++) {
			DV.row(i) = inMesh->vertex(i).cast<double>();
		}
		
		std::vector<std::vector<uint32_t>> vertUsedInTri;
		vertUsedInTri.resize(DV.rows());
		
		std::vector<std::vector<uint32_t>> submeshTriangles; // contains Triangle IDs
		// add all faces from submeshes
		uint32_t startIndex = 0;
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			const CForge::T3DMesh<float>::Submesh* submesh = inMesh->getSubmesh(i);
			std::vector<uint32_t> triIdx;
			
			auto faces = submesh->Faces;
			
			for (uint32_t j = 0; j < faces.size(); j++) {
				triIdx.push_back(startIndex + j);
				Eigen::Vector3i subMVert(faces[j].Vertices[0], faces[j].Vertices[1], faces[j].Vertices[2]);
				for (uint8_t k = 0; k < 3; k++) {
					vertUsedInTri[faces[j].Vertices[k]].push_back(startIndex + j);
				}
				DF.row(startIndex + j) = subMVert;
			} // for every face of a submesh
			
			startIndex += submesh->Faces.size();
			submeshTriangles.push_back(triIdx);
		} // for all submeshes

		//std::cout << "DV\n" << DV << "\n";
		//std::cout << "DF\n" << DF << "\n";
		
		// merge points together
		std::vector<Eigen::Vector3d> DVnoMulVec;
		for (uint32_t i = 0; i < DV.rows(); i++) {
			
			Eigen::Vector3d vert = DV.row(i);
			int32_t idx = DVnoMulVec.size();
			auto itr = std::find(DVnoMulVec.begin(), DVnoMulVec.end(), vert);
			if (itr == DVnoMulVec.end()) { // vertex not contained, create new index
				DVnoMulVec.push_back(vert);
				idx = DVnoMulVec.size() - 1;
			} else
				idx = itr - DVnoMulVec.begin();
			
			// reindex face
			for (uint32_t triIdx : vertUsedInTri[i]) {
				for (uint32_t j = 0; j < 3; j++) {
					if (DF.row(triIdx)[j] == i)
						DF.row(triIdx)[j] = idx;
				}
			}
		} // for vertices
		
		Eigen::MatrixXd DVnoMul(DVnoMulVec.size(),3);
		for (uint32_t i = 0; i < DVnoMulVec.size(); i++) {
			DVnoMul.row(i) = DVnoMulVec[i];
		}
		
		//std::cout << "DVnoMul\n" << DVnoMul << "\n";
		//std::cout << "DV\n" << DV << "\n";
		//std::cout << "DF\n" << DF << "\n";
		bool iglDecRes = igl::decimate(DVnoMul, DF, amount*DF.rows(), DVnoMul, DF, DuF, DuV);
		if (!iglDecRes) {
			std::cout << "an error occured while decimating!\n";
			//std::exit(-1);
		}
		std::cout << "AFTERWARDS:\n";
		//std::cout << "DVnoMul\n" << DVnoMul << "\n";
		//std::cout << "DF\n" << DF << "\n";
		//std::cout << "DuF\n" << DuF << "\n";
		//std::cout << "DuV\n" << DuV << "\n";
				
		//std::cout << "startIndexes:\n";
		//for (uint32_t i : startIndexes)
		//	std::cout << i << ", ";
		//std::cout << "\nDstartIndexes:\n";
		//for (uint32_t i : DstartIndexes)
		//	std::cout << i << ", ";
		//std::cout << "\n";
		
		outMesh->clear();
		std::vector<Eigen::Vector3f> newVerts, newNormals, newTangents, newUVs, newColors;
		std::vector<uint32_t> UV;
		
		// copy all materials
		for (uint32_t i = 0; i < inMesh->materialCount(); i++) {
			T3DMesh<float>::Material* pM = new T3DMesh<float>::Material();
			pM->init(inMesh->getMaterial(i));
			outMesh->addMaterial(pM, false);
		}
		
		uint32_t oldStartSize = 0;
		uint32_t newStartSize = 0;
		// reassemble submeshes
		for (uint32_t i = 0; i < inMesh->submeshCount(); i++) {
			T3DMesh<float>::Submesh* pSubmesh = new T3DMesh<float>::Submesh();
			const T3DMesh<float>::Submesh* pOldSubmesh = inMesh->getSubmesh(i);
			
			std::vector<uint32_t> faces; // list of triangle IDs of Decimated Mesh corresponding to submesh
			uint32_t newTriAmount = 0;
			// only add used faces
			for (uint32_t j = 0; j < submeshTriangles[i].size(); j++) {
				uint32_t triID = submeshTriangles[i][j];
				for (uint32_t k = 0; k < DuF.size(); k++) {
					if (DuF[k] == triID) {
						faces.push_back(triID);
						newTriAmount++;
						break;
					}
				} // for used tris
			} // for old submesh tris
			
			// TODO calculate newUVWs
			//submesh		//faces		//index
			std::vector<std::vector<Eigen::Vector4i>> newUVWs; //TODO rename newUVs conflict
			
			// add materials and set indices of submesh tris
			for (uint32_t j = 0; j < faces.size(); j++) {
				T3DMesh<float>::Face Face;
				
				Face.Material = pOldSubmesh->Faces[faces[j]-oldStartSize].Material;
				for (uint32_t k = 0; k < 4; k++) {
					if (k < 3) {
						int32_t vertID = DF.row(newStartSize + j)[k];
						newVerts.push_back(DVnoMul.row(vertID).cast<float>());
						uint32_t newSize = newVerts.size()-1;
						Face.Vertices[k] = newSize;
						
						newNormals.push_back(Eigen::Vector3f::Zero());
						Face.Normals[k] = newSize;
						
						if (inMesh->textureCoordinatesCount() > 0) {
							newUVs.push_back(inMesh->textureCoordinate(pOldSubmesh->Faces[faces[j]-oldStartSize].UVWs[k]));
							Face.UVWs[k] = newSize;
						}
						if (inMesh->colorCount() > 0) {
							newColors.push_back(inMesh->color(pOldSubmesh->Faces[faces[j] - oldStartSize].Colors[k]));
							Face.Colors[k] = newSize;
						}
						if (inMesh->tangentCount() > 0) {
							newTangents.push_back(inMesh->tangent(pOldSubmesh->Faces[faces[j] - oldStartSize].Tangents[k]));
							Face.Tangents[k] = newSize;
						}
					}
				}//for[face indices]
				
				pSubmesh->Faces.push_back(Face);
			}//for[number of faces]
			outMesh->addSubmesh(pSubmesh, false);
			
			oldStartSize += inMesh->getSubmesh(i)->Faces.size();
			newStartSize += newTriAmount;
		} // for all submeshes

		outMesh->vertices(&newVerts);
		outMesh->normals(&newNormals);
		outMesh->tangents(&newTangents);
		outMesh->textureCoordinates(&newUVs);
		outMesh->colors(&newColors);
		
		outMesh->computePerVertexNormals();
		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() -start).count();
		std::cout << "Decimation Finished, time took: " << double(microseconds)*0.001 << "ms \n";
	} // decimateMesh func

	MeshDecimator::MeshDecimator(void) {
	}//Constructor

	MeshDecimator::~MeshDecimator(void) {
		//clear();
	}//Destructor
}