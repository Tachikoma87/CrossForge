#include "ShapeDeformer.h"

#include <igl/harmonic.h>
#include <igl/writeOBJ.h>
#include <string.h>

//#include <iostream>
//#include <fstream>

using namespace Eigen;

namespace CForge {

	ShapeDeformer::ShapeDeformer(void) : CForgeObject("SurfaceSampler") {

	}//Constructor

	ShapeDeformer::~ShapeDeformer(void) {
		clear();
	}//Destructor

	void ShapeDeformer::init(T3DMesh<float>* pMesh) {
		clear();
		m_pMesh = pMesh;
		mat_Vertices.clear();
	}//initialize

	void ShapeDeformer::clear(void) {
		m_pMesh = nullptr;
		mat_Vertices.clear();
	}//clear

	void ShapeDeformer::release(void) {
		delete this;
	}//release

			//--------------- Shape Deformation

	int ShapeDeformer::binarySearch(int start, int end, int value, std::vector<int32_t> pVec) {
		if (start == end) {
			return start;
		}
		else if (start > end) {
			return 0;
		}
		else {
			int mid = start + (end - start) / 2;

			if (pVec.at(mid) == value)
				return mid;
			else {
				if (pVec.at(mid) > value)
					return binarySearch(start, mid - 1, value, pVec);
				else return binarySearch(mid + 1, end, value, pVec);
			}
		}

	}

	void ShapeDeformer::setSamplePoints(void) {
		std::string filepath("Assets/tmp/samplePoints1000.txt");
		int numSamplePoints;

		std::ifstream myfile(filepath);

		if (!myfile.is_open()) {
			std::cout << "Unable to open file\n";
		}
		else {
			myfile >> numSamplePoints;
			this->m_SamplePoints.resize(numSamplePoints);

			for (int i = 0; i < numSamplePoints; i++)
			{
				myfile >> m_SamplePoints(i);
			}

			myfile.close();

		}
	}

	void ShapeDeformer::setFaceMatrix(void) {

		if (m_pMesh->boneSubmeshCount() == 0) {
			m_pMesh->initBoneSubmeshes(0.5f, true);
		}

		int32_t faceCount = m_pMesh->getBoneSubmesh(0)->Faces.size();

		this->mat_Faces.resize(faceCount, 3);

		for (int i = 0; i < faceCount; i++) {
			T3DMesh<float>::Face currentFace = m_pMesh->getBoneSubmesh(0)->Faces.at(i);
			mat_Faces.row(i) = Eigen::RowVector3i(currentFace.Vertices[0], currentFace.Vertices[1], currentFace.Vertices[2]);
		}

	}

	void ShapeDeformer::sortSamplePoints(Eigen::VectorXi bones) {

		for (int j = 0; j < bones.rows(); j++) {
			T3DMesh<float>::Bone* b = m_pMesh->getBone(bones(j));

			std::vector<int32_t> pPointsInside;

			for (int32_t i = 0; i < m_SamplePoints.rows(); i++) {
				int32_t possibleIndex = binarySearch(0, b->VertexInfluences.size() - 1, this->m_SamplePoints(i), b->VertexInfluences);

				if (b->VertexInfluences.at(possibleIndex) == m_SamplePoints(i) && b->VertexWeights.at(possibleIndex) > 0.50f) {
					pPointsInside.push_back(i);
				}
			}
			/*
			Eigen::VectorXi vPointsInside(pPointsInside.size());
			for (int32_t i = 0; i < pPointsInside.size(); i++) {
				vPointsInside(i) = pPointsInside.at(i);
			}

			pPointsInside.clear();*/
			this->m_SamplePointsInBones.push_back(pPointsInside);
		}

	}

	void ShapeDeformer::prepareShapeDeformation(Eigen::VectorXi importantBones, int32_t AnimationID, int32_t startFrame, int32_t endFrame) {
		this->setSamplePoints();
		this->sortSamplePoints(importantBones);
		this->setFaceMatrix();
		m_pMesh->setSkinningMats(startFrame, endFrame, AnimationID, m_pMesh->rootBone()->ID);
	}

	Eigen::Vector2f ShapeDeformer::getOptimalCapsuleRadius(T3DMesh<float>::Line Capsule1, float capsuleRadius1, T3DMesh<float>::Line Capsule2, float capsuleRadius2) {

		Eigen::Vector3f A_1 = Capsule1.p;
		Eigen::Vector3f B_1 = A_1 + Capsule1.direction;

		Eigen::Vector3f A_2 = Capsule2.p;
		Eigen::Vector3f B_2 = A_2 + Capsule2.direction;

		Eigen::Vector3f v0 = A_2 - A_1;
		Eigen::Vector3f v1 = B_2 - A_1;
		Eigen::Vector3f v2 = A_2 - B_1;
		Eigen::Vector3f v3 = B_2 - B_1;

		// squared distances:
		float d0 = v0.dot(v0);
		float d1 = v1.dot(v1);
		float d2 = v2.dot(v2);
		float d3 = v3.dot(v3);

		Eigen::Vector3f best1;
		if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		{
			best1 = B_1;
		}
		else
		{
			best1 = A_1;
		}

		// select point on capsule B line segment nearest to best potential endpoint on A capsule:
		Eigen::Vector3f best2 = m_pMesh->closestPointOnLineSegment(A_2, B_2, best1);

		// now do the same for capsule A segment:
		best1 = m_pMesh->closestPointOnLineSegment(A_1, B_1, best2);

		Eigen::Vector3f best1best2 = best2 - best1;
		best1best2.normalize();
		Eigen::Vector3f temp1 = best1 + capsuleRadius1 * best1best2;
		Eigen::Vector3f temp2 = best2 - capsuleRadius2 * best1best2;

		Eigen::Vector3f center = temp1 + (temp2 - temp1) / 2;

		Eigen::Vector2f Rval;
		Rval.x() = m_pMesh->vectorLength(center - best1);
		Rval.y() = m_pMesh->vectorLength(center - best2);

		return Rval;
	}

	Eigen::Vector3f ShapeDeformer::bestLocationSamplePoint(T3DMesh<float>::Line Capsule, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame) {

		//Eigen::Vector3f pPoint = m_pMesh->getUpdatedVertexPosition(frame, vertexID);

		Eigen::Vector3f A = Capsule.p;
		Eigen::Vector3f B = A + Capsule.direction;
		//pPoint belongs to Capsule 1
		//A and B belong to Capsule 2
		Eigen::Vector3f center = m_pMesh->closestPointOnLineSegment(A, B, pPoint);

		//distance between Center and pPoint should be >=capsuleRadius in non-colliding/just-right case
		//else pPoint has to be moved to outline of Capsule 2
		Eigen::Vector3f distance = pPoint - center;
		float distanceToCenter = m_pMesh->vectorLength(distance);

		if (distanceToCenter > optimalCapsuleRadius) {
			return pPoint;
		}

		float translationDistance = (optimalCapsuleRadius - distanceToCenter) / 2.0f;
		//Eigen::Vector3f pNormal = m_pMesh->getUpdatedNormal(frame, vertexID);
		distance.normalize();
		//Eigen::Vector3f Rval = pPoint + (-1) *translationDistance * distance;
		Eigen::Vector3f Rval = pPoint + translationDistance * distance;
		/*Eigen::Vector3f Rval;
		Eigen::Vector3f Vertex = m_pMesh->getUpdatedVertexPosition(frame, vertexID);
		Eigen::Vector3f Normal = m_pMesh->getUpdatedNormal(frame, vertexID);

		Rval = Vertex - Eigen::Vector3f(1,0,1);*/

		return Rval;
	}

	Eigen::Vector3f ShapeDeformer::bestLocationSamplePoint2(T3DMesh<float>::Line Capsule1, T3DMesh<float>::Line Capsule2, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame) {
		Eigen::Vector3f Rval = pPoint;
		Eigen::Vector3f A1 = Capsule1.p;
		Eigen::Vector3f B1 = A1 + Capsule1.direction;

		Eigen::Vector3f A2 = Capsule2.p;
		Eigen::Vector3f B2 = A2 + Capsule2.direction;
		//pPoint belongs to Capsule 1
		//A and B belong to Capsule 2
		Eigen::Vector3f center1 = m_pMesh->closestPointOnLineSegment(A1, B1, pPoint);
		Eigen::Vector3f center2 = m_pMesh->closestPointOnLineSegment(A2, B2, pPoint);

		Eigen::Vector3f distance = pPoint - center2;
		Eigen::Vector3f direction = center1 - pPoint;
		direction.normalize();
		float dist = m_pMesh->vectorLength(distance);

		if (dist < optimalCapsuleRadius) {
			Rval = pPoint + (optimalCapsuleRadius - dist) / 2.0f * direction;
		}

		return Rval;
	}

	/*Eigen::Vector3f ShapeDeformer::bestLocationSamplePoint3(T3DMesh<float>::Line Capsule, int32_t vertexID, int32_t TriIDs[3], int32_t frame) {

		Eigen::Vector3f pPoint = m_pMesh->getUpdatedVertexPosition(frame, vertexID);
		Eigen::Vector3f Tri[3];
		Tri[0] = m_pMesh->getUpdatedVertexPosition(frame, TriIDs[0]);
		Tri[1] = m_pMesh->getUpdatedVertexPosition(frame, TriIDs[1]);
		Tri[2] = m_pMesh->getUpdatedVertexPosition(frame, TriIDs[2]);

		Eigen::Vector3f Schwerpunkt = (Tri[0] + Tri[1] + Tri[2]) / 3.0f;
		Eigen::Vector3f A = Capsule.p;
		Eigen::Vector3f B = A + Capsule.direction;
		//pPoint belongs to Capsule 1
		//A and B belong to Capsule 2
		Eigen::Vector3f center = m_pMesh->closestPointOnLineSegment(A, B, pPoint);

		//distance between Center and pPoint should be >=capsuleRadius in non-colliding/just-right case
		//else pPoint has to be moved to outline of Capsule 2
		Eigen::Vector3f distance1 = pPoint - center;
		Eigen::Vector3f distance2 = Schwerpunkt - center;
		float distance1Length = m_pMesh->vectorLength(distance1);
		float distance2Length = m_pMesh->vectorLength(distance2);

		if (distance1Length > distance2Length)
			return pPoint;

		float translationDistance = distance2Length - distance1Length;
		distance1.normalize();
		Eigen::Vector3f Rval = pPoint + translationDistance * distance1;
		return Rval;
	}*/

	Eigen::Vector3f ShapeDeformer::bestLocationSamplePoint3(T3DMesh<float>::Line Capsule1, T3DMesh<float>::Line Capsule2, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame) {
		Eigen::Vector3f Rval = pPoint;
		Eigen::Vector3f A1 = Capsule1.p;
		Eigen::Vector3f B1 = A1 + Capsule1.direction;

		Eigen::Vector3f A2 = Capsule2.p;
		Eigen::Vector3f B2 = A2 + Capsule2.direction;
		//pPoint belongs to Capsule 1
		//A and B belong to Capsule 2
		Eigen::Vector3f center1 = m_pMesh->closestPointOnLineSegment(A1, B1, pPoint);
		Eigen::Vector3f center2 = m_pMesh->closestPointOnLineSegment(A2, B2, pPoint);

		Eigen::Vector3f distance = pPoint - center2;
		Eigen::Vector3f direction = center1 - pPoint;
		direction.normalize();
		float dist = m_pMesh->vectorLength(distance);

		if (dist < optimalCapsuleRadius) {
			printf("ControlPoint going to be moved\n");
			Rval = pPoint + ((optimalCapsuleRadius - dist) * 0.6f) * direction;
		}

		return Rval;
	}

	Eigen::Vector3f ShapeDeformer::bestLocationSamplePoint4(T3DMesh<float>::Line Capsule1, T3DMesh<float>::Line Capsule2, float optimalCapsuleRadius, Eigen::Vector3f pPoint, int32_t frame) {
		Eigen::Vector3f Rval = pPoint;
		Eigen::Vector3f A1 = Capsule1.p;
		Eigen::Vector3f B1 = A1 + Capsule1.direction;

		Eigen::Vector3f A2 = Capsule2.p;
		Eigen::Vector3f B2 = A2 + Capsule2.direction;
		//pPoint belongs to Capsule 1
		//A and B belong to Capsule 2
		Eigen::Vector3f center1 = m_pMesh->closestPointOnLineSegment(A1, B1, pPoint);
		Eigen::Vector3f center2 = m_pMesh->closestPointOnLineSegment(A2, B2, pPoint);

		Eigen::Vector3f distance = pPoint - center2;
		Eigen::Vector3f direction = center1 - pPoint;
		direction.normalize();
		float dist = m_pMesh->vectorLength(distance);

		if (dist < optimalCapsuleRadius) {
			Rval = pPoint + ((optimalCapsuleRadius - dist)) * direction;
		}

		return Rval;
	}

	Eigen::Vector3f ShapeDeformer::getcloseVert(T3DMesh<float>::Line Capsule, int32_t BoneID, Eigen::Vector3f pVertex, int frame) {
		float Rval;
		Eigen::Vector3f A = Capsule.p;
		Eigen::Vector3f B = A + Capsule.direction;
		//Eigen::Vector3f pVertex = m_pMesh->getUpdatedVertexPosition(frame, vertexID);
		Eigen::Vector3f closestPointOnBone = m_pMesh->closestPointOnLineSegment(A, B, pVertex);
		std::vector<int32_t> VerticesInSubmesh = m_pMesh->getVerticesInSubmesh(BoneID);

		Eigen::Vector3f Original = pVertex - closestPointOnBone;

		Eigen::Vector3f vertWithSmallestAngle = m_pMesh->getUpdatedVertexPositionOld(frame, VerticesInSubmesh.at(0));
		Eigen::Vector3f tempVector = vertWithSmallestAngle - closestPointOnBone;
		float smallestAngle = acosf((Original.dot(tempVector)) / (m_pMesh->vectorLength(Original) * m_pMesh->vectorLength(tempVector)));


		for (int i = 1; i < VerticesInSubmesh.size(); i++) {
			Eigen::Vector3f tempVert = m_pMesh->getUpdatedVertexPositionOld(frame, VerticesInSubmesh.at(i));
			tempVector = tempVert - closestPointOnBone;
			if (m_pMesh->vectorLength(tempVector) > m_pMesh->vectorLength(Original)) {
				float tempAngle = acosf((Original.dot(tempVector)) / (m_pMesh->vectorLength(Original) * m_pMesh->vectorLength(tempVector)));

				if (tempAngle < smallestAngle) {
					smallestAngle = tempAngle;
					vertWithSmallestAngle = tempVert;
				}
			}

		}

		return vertWithSmallestAngle;
	}

	Eigen::Vector3f ShapeDeformer::getcloseVert2(T3DMesh<float>::Line Capsule, int32_t BoneID, Eigen::Vector3f pVertex, int frame) {
		//float Rval;
		Eigen::Vector3f A = Capsule.p;
		Eigen::Vector3f B = A + Capsule.direction;
		//Eigen::Vector3f pVertex = m_pMesh->getUpdatedVertexPosition(frame, vertexID);
		Eigen::Vector3f closestPointOnBone = m_pMesh->closestPointOnLineSegment(A, B, pVertex);
		int32_t faceCount = m_pMesh->getBoneSubmesh(BoneID + 1)->Faces.size();
		Eigen::Vector3f Original = pVertex - closestPointOnBone;
		float beginningDepth = m_pMesh->vectorLength(Original);
		Original.normalize();
		T3DMesh<float>::Line OriginalLine(closestPointOnBone, Original);
		Eigen::Vector3f bestGuess = closestPointOnBone;

		for (int i = 0; i < faceCount; i++) {
			T3DMesh<float>::Face currentFace = m_pMesh->getBoneSubmesh(BoneID + 1)->Faces.at(i);
			Eigen::Vector3f Tri[3];
			Tri[0] = m_pMesh->getUpdatedVertexPositionOld(frame, currentFace.Vertices[0]);
			Tri[1] = m_pMesh->getUpdatedVertexPositionOld(frame, currentFace.Vertices[1]);
			Tri[2] = m_pMesh->getUpdatedVertexPositionOld(frame, currentFace.Vertices[2]);

			float templength = m_pMesh->lineTriangleIntersection(OriginalLine, Tri);
			if (templength > 0) {
				Eigen::Vector3f temp = closestPointOnBone + templength * Original;
				if (m_pMesh->vertInsideTriangle(Tri, temp) == true) {
					bestGuess = temp;
				}
			}
		}
		return bestGuess;
	}

	float ShapeDeformer::getAccurateCapsuleRadius(T3DMesh<float>::Line Capsule, int32_t BoneID, Eigen::Vector3f pVertex, int frame) {
		float Rval;
		Eigen::Vector3f A = Capsule.p;
		Eigen::Vector3f B = A + Capsule.direction;
		//Eigen::Vector3f pVertex = m_pMesh->getUpdatedVertexPosition(frame, vertexID);
		Eigen::Vector3f closestPointOnBone = m_pMesh->closestPointOnLineSegment(A, B, pVertex);
		std::vector<int32_t> VerticesInSubmesh = m_pMesh->getVerticesInSubmesh(BoneID);
		printf("%d Verts In Submesh %d\n", VerticesInSubmesh.size(), BoneID + 1);
		Eigen::Vector3f Original = pVertex - closestPointOnBone;

		Eigen::Vector3f vertWithSmallestAngle = m_pMesh->getUpdatedVertexPositionOld(frame, VerticesInSubmesh.at(0));
		Eigen::Vector3f tempVector = vertWithSmallestAngle - closestPointOnBone;
		float smallestAngle = acosf((Original.dot(tempVector)) / (m_pMesh->vectorLength(Original) * m_pMesh->vectorLength(tempVector)));

		for (int i = 1; i < VerticesInSubmesh.size(); i++) {
			Eigen::Vector3f tempVert = m_pMesh->getUpdatedVertexPositionOld(frame, VerticesInSubmesh.at(i));
			tempVector = tempVert - closestPointOnBone;
			float tempAngle = acosf((Original.dot(tempVector)) / (m_pMesh->vectorLength(Original) * m_pMesh->vectorLength(tempVector)));

			if (tempAngle < smallestAngle) {
				smallestAngle = tempAngle;
				vertWithSmallestAngle = tempVert;
			}
		}

		Eigen::Vector3f capsulePoint = m_pMesh->closestPointOnLineSegment(A, B, vertWithSmallestAngle);
		Rval = m_pMesh->vectorLength(capsulePoint - vertWithSmallestAngle);

		return Rval;
	}

	int ShapeDeformer::findInUnsortedVector(std::vector<int32_t> vector, int32_t value) {

		int Rval = -1;
		bool found = false;
		int index = 0;

		while (index < vector.size() && found == false) {
			if (vector.at(index) == value) {
				found = true;
				Rval = index;
			}
			else {
				index++;
			}
		}

		return Rval;
	}

	std::vector<int32_t> ShapeDeformer::findAdditionalVertices(T3DMesh<float>::Submesh* Mesh, std::vector<int32_t> goodVertices, std::vector<int32_t> forbiddenVertices, std::vector<int32_t> trianglesToCheck) {

		int currentTriToCheck = 0;
		int lastSize = trianglesToCheck.size();

		while (trianglesToCheck.size() > 0) {

			//go from beginning to end
			while (currentTriToCheck < trianglesToCheck.size()) {
				//3 verts of a Triangle
				int32_t verts[3];
				verts[0] = Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[0];
				verts[1] = Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[1];
				verts[2] = Mesh->Faces.at(trianglesToCheck.at(currentTriToCheck)).Vertices[2];
				//bad ones are outside of BB or outside of other Mesh
				bool foundBad[3] = { false, false, false };
				for (int j = 0; j < 3; j++) {
					int index = findInUnsortedVector(forbiddenVertices, verts[j]);
					if (index > -1) {
						foundBad[j] = true;
					}
					else
						foundBad[j] = false;
				}

				//good ones are inside other mesh
				bool foundGood[3] = { false, false, false };

				if (foundBad[0] == false) {
					int index = findInUnsortedVector(goodVertices, verts[0]);
					if (index > -1) {
						foundGood[0] = true;
					}
					else
						foundGood[0] = false;
				}
				if (foundBad[1] == false) {
					int index = findInUnsortedVector(goodVertices, verts[1]);
					if (index > -1) {
						foundGood[1] = true;
					}
					else
						foundGood[1] = false;
				}
				if (foundBad[2] == false) {
					int index = findInUnsortedVector(goodVertices, verts[2]);
					if (index > -1) {
						foundGood[2] = true;
					}
					else
						foundGood[2] = false;
				}


				if (foundBad[0] == false && foundBad[1] == false && foundBad[2] == false) {
					if (foundGood[0] || foundGood[1] || foundGood[2]) {

						if (foundGood[0] == false) {
							goodVertices.push_back(verts[0]);
						}

						if (foundGood[1] == false) {
							goodVertices.push_back(verts[1]);
						}

						if (foundGood[2] == false) {
							goodVertices.push_back(verts[2]);
						}

						trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);


					}
					else {
						currentTriToCheck++;
					}
				}
				else {
					trianglesToCheck.erase(trianglesToCheck.begin() + currentTriToCheck);
				}

			}
			if (lastSize > trianglesToCheck.size()) {
				currentTriToCheck = 0;
				lastSize = trianglesToCheck.size();
			}
			else {
				trianglesToCheck.clear();
			}

		}

		return goodVertices;
	}

	std::vector<std::vector<int32_t>> ShapeDeformer::findAffectedControlPoints(T3DMesh<float>::Submesh* Submesh1, T3DMesh<float>::Submesh* Submesh2, std::vector<bool> checkMesh1, std::vector<bool> checkMesh2, std::vector<Eigen::Vector2i> affectedTriangles, int frame) {
		std::vector<int32_t> ControlPointsMesh1;
		std::vector<int32_t> ControlPointsMesh2;
		bool requested[4] = { true, true, false, false };
		std::vector<std::vector<int32_t>> vertsInsideMesh = m_pMesh->findAllInside(Submesh1, Submesh2, checkMesh1, checkMesh2, affectedTriangles, frame, requested, true);

		for (int i = 0; i < vertsInsideMesh.at(0).size(); i++) {

			bool found = false;
			int32_t index = 0;
			while (found == false && index < m_SamplePointsInBones.at(0).size()) {
				if (m_SamplePoints(m_SamplePointsInBones.at(0).at(index)) == vertsInsideMesh.at(0).at(i)) {
					found = true;
					ControlPointsMesh1.push_back(m_SamplePointsInBones.at(0).at(index));
				}
				else {
					index++;
				}
			}

		}

		for (int i = 0; i < vertsInsideMesh.at(1).size(); i++) {

			bool found = false;
			int32_t index = 0;
			while (found == false && index < m_SamplePointsInBones.at(1).size()) {
				if (m_SamplePoints(m_SamplePointsInBones.at(1).at(index)) == vertsInsideMesh.at(1).at(i)) {
					found = true;
					ControlPointsMesh2.push_back(m_SamplePointsInBones.at(1).at(index));
				}
				else {
					index++;
				}
			}

		}
		std::vector<std::vector<int32_t>> Rval;
		Rval.push_back(ControlPointsMesh1);
		Rval.push_back(ControlPointsMesh2);
		
		Eigen::MatrixXf testV = m_pMesh->getTriDeformationMatrix(frame);

		for (int i = 0; i < vertsInsideMesh.at(0).size(); i++) {
			testV(vertsInsideMesh.at(0).at(i), 0) = testV(vertsInsideMesh.at(0).at(i), 0) + 1.0f;
		}
		for (int i = 0; i < vertsInsideMesh.at(1).size(); i++) {
			testV(vertsInsideMesh.at(1).at(i), 0) = testV(vertsInsideMesh.at(1).at(i), 0) - 1.0f;
		}

		std::string filename = "Whatever" + std::to_string(frame) + ".obj";
		igl::writeOBJ(filename, testV, mat_Faces);


		vertsInsideMesh.at(0).clear();
		vertsInsideMesh.at(1).clear();
		vertsInsideMesh.clear();
		ControlPointsMesh1.clear();
		ControlPointsMesh2.clear();

		return Rval;
		/*
		std::vector<int32_t> ControlPointsMesh1;
		std::vector<int32_t> ControlPointsMesh2;

		std::vector<int32_t> vertsInsideMesh2; //verts of Mesh1 that are inside Mesh2
		std::vector<int32_t> vertsInsideMesh1; //verts of Mesh2 that are inside Mesh1
		std::vector<int32_t> vertsInsideMesh2Full; //verts of Mesh1 that are inside Mesh2
		std::vector<int32_t> vertsInsideMesh1Full; //verts of Mesh2 that are inside Mesh1
		std::vector<int32_t> vertsOutsideMesh2; //verts of Mesh1 that are outside Mesh2
		std::vector<int32_t> vertsOutsideMesh1; //verts of Mesh2 that are outside Mesh1
		std::vector<int32_t> TrianglesToCheck1;
		std::vector<int32_t> TrianglesToCheck2;

		//Vertices of Triangles outside of intersection area can't possibly be inside the other mesh
		//sort these Vertices into vertsOutsideMesh, act as boundary when searching for vertices inside mesh
		for (int i = 0; i < checkMesh1.size(); i++) {
			if (checkMesh1.at(i) == false) {
				int32_t Vertex[3] = { Submesh1->Faces.at(i).Vertices[0],
				Submesh1->Faces.at(i).Vertices[1],
				Submesh1->Faces.at(i).Vertices[2] };

				for (int j = 0; j < 3; j++) {
					bool found = m_pMesh->findInVector(vertsOutsideMesh2, Vertex[j]);

					if (found == false) {
						vertsOutsideMesh2.push_back(Vertex[j]);
					}
				}
			}
		}

		for (int i = 0; i < checkMesh2.size(); i++) {
			if (checkMesh2.at(i) == false) {
				int32_t Vertex[3] = { Submesh2->Faces.at(i).Vertices[0],
				Submesh2->Faces.at(i).Vertices[1],
				Submesh2->Faces.at(i).Vertices[2] };

				for (int j = 0; j < 3; j++) {
					int32_t index = findInUnsortedVector(vertsOutsideMesh1, Vertex[j]);
					if (index == -1) {
						vertsOutsideMesh1.push_back(Vertex[j]);
					}

				}
			}
		}

		for (int i = 0; i < affectedTriangles.size(); i++) {

			Eigen::Vector3f Tri1[3];
			int32_t Vertex1[3] = { m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[0],
			m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[1],
			m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[2] };

			Eigen::Vector3f Tri2[3];
			int32_t Vertex2[3] = { m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0],
			m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1],
			m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2] };

			Tri1[0] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex1[0]);
			Tri1[1] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex1[1]);
			Tri1[2] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex1[2]);

			Tri2[0] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex2[0]);
			Tri2[1] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex2[1]);
			Tri2[2] = m_pMesh->getUpdatedVertexPositionOld(frame, Vertex2[2]);

			Eigen::Vector3f TriNormal1 = m_pMesh->get_normal(Tri1[0], Tri1[1], Tri1[2]);
			Eigen::Vector3f TriNormal2 = m_pMesh->get_normal(Tri2[0], Tri2[1], Tri2[2]);

			//sort the vertices of intersecting triangles depending on their location inside or outside the
			//other mesh, Vertices of Mesh1 that are outside of Mesh2 are in vertsOutsideMesh2 etc
			for (int j = 0; j < 3; j++) {

				T3DMesh<float>::Line pLine1(Tri1[j], TriNormal2);
				T3DMesh<float>::Line pLine2(Tri2[j], TriNormal1);
				//Check if this performs correctly
				float temp1 = m_pMesh->lineTriangleIntersection(pLine1, Tri2);
				float temp2 = m_pMesh->lineTriangleIntersection(pLine2, Tri1);

				if (temp1 < 0) {
					bool found = m_pMesh->findInVector(vertsInsideMesh2, Vertex1[j]);
					if (found == false) {
						found = m_pMesh->findInVector(vertsOutsideMesh2, Vertex1[j]);
						if (found == false) {
							vertsOutsideMesh2.push_back(Vertex1[j]);
						}
					}
				}
				else {

					bool found = m_pMesh->findInVector(vertsInsideMesh2, Vertex1[j]);
					if (found == false) {
						vertsInsideMesh2.push_back(Vertex1[j]);
					}

					int index = findInUnsortedVector(vertsOutsideMesh2, Vertex1[j]);
					if (index > -1) {
						vertsOutsideMesh2.erase(vertsOutsideMesh2.begin() + index);
					}
				}

				if (temp2 < 0) {
					bool found = m_pMesh->findInVector(vertsInsideMesh1, Vertex2[j]);
					if (found == false) {
						found = m_pMesh->findInVector(vertsOutsideMesh1, Vertex2[j]);
						if (found == false) {
							vertsOutsideMesh1.push_back(Vertex2[j]);
						}
					}
				}
				else {

					bool found = m_pMesh->findInVector(vertsInsideMesh1, Vertex2[j]);
					if (found == false) {
						vertsInsideMesh1.push_back(Vertex2[j]);
					}

					int index = findInUnsortedVector(vertsOutsideMesh1, Vertex2[j]);
					if (index > -1) {
						vertsOutsideMesh1.erase(vertsOutsideMesh1.begin() + index);
					}
				}
			}
		}

		for (int i = 0; i < checkMesh1.size(); i++) {
			if (checkMesh1.at(i) == true) {
				int32_t Vertex[3] = { Submesh1->Faces.at(i).Vertices[0],
				Submesh1->Faces.at(i).Vertices[1],
				Submesh1->Faces.at(i).Vertices[2] };
				bool foundOutside[3];// = { false, false, false };
				for (int j = 0; j < 3; j++) {
					foundOutside[j] = m_pMesh->findInVector(vertsOutsideMesh2, Vertex[j]);
				}
				if (foundOutside[0] == false && foundOutside[1] == false && foundOutside[2] == false) {
					TrianglesToCheck1.push_back(i);
				}
			}
		}

		for (int i = 0; i < checkMesh2.size(); i++) {
			if (checkMesh2.at(i) == true) {
				int32_t Vertex[3] = { Submesh2->Faces.at(i).Vertices[0],
				Submesh2->Faces.at(i).Vertices[1],
				Submesh2->Faces.at(i).Vertices[2] };
				bool foundOutside[3] = { false, false, false };
				for (int j = 0; j < 3; j++) {
					int index = findInUnsortedVector(vertsOutsideMesh1, Vertex[j]);
					if (index > -1) {
						foundOutside[j] = true;
					}
				}
				if (foundOutside[0] == false && foundOutside[1] == false && foundOutside[2] == false) {
					TrianglesToCheck2.push_back(i);
				}
			}
		}

		vertsInsideMesh2Full = findAdditionalVertices(Submesh1, vertsInsideMesh2, vertsOutsideMesh2, TrianglesToCheck1);
		vertsInsideMesh1Full = findAdditionalVertices(Submesh2, vertsInsideMesh1, vertsOutsideMesh1, TrianglesToCheck2);

		vertsInsideMesh1.clear();
		vertsInsideMesh2.clear();

		for (int i = 0; i < vertsInsideMesh2Full.size(); i++) {

			bool found = false;
			int32_t index = 0;
			while (found == false && index < m_SamplePointsInBones.at(0).size()) {
				if (m_SamplePoints(m_SamplePointsInBones.at(0).at(index)) == vertsInsideMesh2Full.at(i)) {
					found = true;
					ControlPointsMesh1.push_back(m_SamplePointsInBones.at(0).at(index));
				}
				else {
					index++;
				}
			}

		}

		for (int i = 0; i < vertsInsideMesh1Full.size(); i++) {

			bool found = false;
			int32_t index = 0;
			while (found == false && index < m_SamplePointsInBones.at(1).size()) {
				if (m_SamplePoints(m_SamplePointsInBones.at(1).at(index)) == vertsInsideMesh1Full.at(i)) {
					found = true;
					ControlPointsMesh2.push_back(m_SamplePointsInBones.at(1).at(index));
				}
				else {
					index++;
				}
			}

		}

		std::vector<std::vector<int32_t>> Rval;
		Rval.push_back(ControlPointsMesh1);
		Rval.push_back(ControlPointsMesh2);

		Eigen::MatrixXf testV = m_pMesh->getTriDeformationMatrix(frame);

		for (int i = 0; i < vertsInsideMesh1Full.size(); i++) {
			testV(vertsInsideMesh1Full.at(i), 0) = testV(vertsInsideMesh1Full.at(i), 0) - 0.2f;
		}
		for (int i = 0; i < vertsInsideMesh2Full.size(); i++) {
			testV(vertsInsideMesh2Full.at(i), 0) = testV(vertsInsideMesh2Full.at(i), 0) + 0.2f;
		}

		std::string filename = "Whatever" + std::to_string(frame) + ".obj";
		igl::writeOBJ(filename, testV, mat_Faces);

		vertsInsideMesh1Full.clear();
		vertsInsideMesh2Full.clear();
		vertsOutsideMesh1.clear();
		vertsOutsideMesh2.clear();
		TrianglesToCheck1.clear();
		TrianglesToCheck2.clear();

		return Rval;*/
	}

	Eigen::MatrixXf ShapeDeformer::collisionTestShapeDeformation(int Submesh1Index, int Submesh2Index, int frame, int AnimationID, int32_t method, Eigen::MatrixXf U_bc, Eigen::MatrixXf MeshVertices) {

		//Submesh, where Bones shall be updated in case of collision
		T3DMesh<float>::Submesh* Submesh1 = m_pMesh->getBoneSubmesh(Submesh1Index);
		T3DMesh<float>::Bone* boneMesh1 = m_pMesh->getBone(Submesh1Index - 1);
		//Submesh, that might collide with Submesh1
		T3DMesh<float>::Submesh* Submesh2 = m_pMesh->getBoneSubmesh(Submesh2Index);
		T3DMesh<float>::Bone* boneMesh2 = m_pMesh->getBone(Submesh2Index - 1);
		//Submeshes need updated Vertex Positions for AABB
		T3DMesh<float>::AABB mesh1BB = Submesh1->initBB(MeshVertices);
		T3DMesh<float>::AABB mesh2BB = Submesh2->initBB(MeshVertices);

		bool spheresIntersect, boxesIntersect, trianglesIntersect = false;

		//Sphere Test -------------------------
		spheresIntersect = m_pMesh->boundingSphereIntersectionTest(mesh1BB, mesh2BB);
		if (!spheresIntersect) return U_bc;

		//Box Test------------------------------
		Eigen::MatrixXf LapMinMax = m_pMesh->determineAABBminmax(mesh1BB, mesh2BB);
		boxesIntersect = true;

		if (LapMinMax(0, 0) > LapMinMax(1, 0)) boxesIntersect = false;
		if (LapMinMax(0, 1) > LapMinMax(1, 1)) boxesIntersect = false;
		if (LapMinMax(0, 2) > LapMinMax(1, 2)) boxesIntersect = false;

		if (!boxesIntersect) return U_bc;

		//---triangle Intersection Test of Triangles inside LapMinMax

		std::vector<Eigen::Vector2i> affectedTriangles;

		std::vector <bool> checkMesh1;
		std::vector <bool> checkMesh2;

		for (auto i : Submesh2->Faces) {
			Eigen::Vector3f tri[3];
			tri[0] = Eigen::Vector3f(MeshVertices.row(i.Vertices[0]));
			tri[1] = Eigen::Vector3f(MeshVertices.row(i.Vertices[1]));
			tri[2] = Eigen::Vector3f(MeshVertices.row(i.Vertices[2]));

			checkMesh2.push_back(m_pMesh->inBoundingBox(tri, Eigen::Vector3f(LapMinMax.row(0)), Eigen::Vector3f(LapMinMax.row(1))));
		}

		for (auto i : Submesh1->Faces) {
			Eigen::Vector3f tri[3];
			tri[0] = Eigen::Vector3f(MeshVertices.row(i.Vertices[0]));
			tri[1] = Eigen::Vector3f(MeshVertices.row(i.Vertices[1]));
			tri[2] = Eigen::Vector3f(MeshVertices.row(i.Vertices[2]));

			checkMesh1.push_back(m_pMesh->inBoundingBox(tri, Eigen::Vector3f(LapMinMax.row(0)), Eigen::Vector3f(LapMinMax.row(1))));
		}

		//Triangle Test
		for (int i = 0; i < Submesh1->Faces.size(); i++) {
			if (checkMesh1.at(i) == false)
				continue;

			Eigen::Vector3f tri1[3];
			tri1[0] = Eigen::Vector3f(MeshVertices.row(Submesh1->Faces.at(i).Vertices[0]));
			tri1[1] = Eigen::Vector3f(MeshVertices.row(Submesh1->Faces.at(i).Vertices[1]));
			tri1[2] = Eigen::Vector3f(MeshVertices.row(Submesh1->Faces.at(i).Vertices[2]));

			for (int j = 0; j < Submesh2->Faces.size(); j++) {

				if (checkMesh2.at(j) == false) {
					continue;
				}

				Eigen::Vector3f tri2[3];
				tri2[0] = Eigen::Vector3f(MeshVertices.row(Submesh2->Faces.at(j).Vertices[0]));
				tri2[1] = Eigen::Vector3f(MeshVertices.row(Submesh2->Faces.at(j).Vertices[1]));
				tri2[2] = Eigen::Vector3f(MeshVertices.row(Submesh2->Faces.at(j).Vertices[2]));

				if (m_pMesh->triangleIntersection(tri1, tri2) == false)
					continue;

				// Sonst: wir haben (endlich) einen Schnitt gefunden!
				affectedTriangles.push_back(Eigen::Vector2i(Submesh1->FaceID.at(i), Submesh2->FaceID.at(j)));
			}
		}

		if (affectedTriangles.size() > 0) {
			/*for (int i = 0; i < affectedTriangles.size(); i++) {
				for (int v = 0; v < 3; v++) {
					int32_t VertexID = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[v];
					int32_t m = 0;
					bool found = false;
					while (found == false && m < m_SamplePointsInBones.at(0).size()) {
						int32_t samplePointID = m_SamplePoints(m_SamplePointsInBones.at(0).at(m));
						if (VertexID == samplePointID)
							found = true;
						else {
							m++;
						}
					}
					if (found == true) {

						//int32_t TriIDs[3];
						//TriIDs[0] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[0];
						//TriIDs[1] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[1];
						//TriIDs[2] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[2];

						T3DMesh<float>::Line Capsule2 = m_pMesh->getCapsuleDirection(boneMesh2, AnimationID, frame);
						//T3DMesh<float>::Line Capsule1 = m_pMesh->getCapsuleDirection(boneMesh1, AnimationID, frame);

						//float radius1 = boneMesh1->capsuleRadius;
						float radius2 = 14.0f;
						//float radius2 = boneMesh2->capsuleRadius;
						//Eigen::Vector2f optimalRadius = getOptimalCapsuleRadius(Capsule1, radius1, Capsule2, radius2);
						Eigen::Vector2f optimalRadius;
						optimalRadius(0) = 9.0f;
						optimalRadius(1) = 9.0f;

						float accurateRadius = getAccurateCapsuleRadius(Capsule2, Submesh2Index - 1, m_SamplePoints(m_SamplePointsInBones.at(0).at(m)), frame);
						printf("Radius: %d\n", accurateRadius);
						Eigen::Vector3f newPosition = bestLocationSamplePoint(Capsule2, accurateRadius, m_SamplePoints(m_SamplePointsInBones.at(0).at(m)), frame);

						//Eigen::Vector3f newPosition = bestLocationSamplePoint(Capsule2, optimalRadius(1), m_SamplePoints(m_SamplePointsInBones.at(0).at(m)), frame);
					//	Eigen::Vector3f newPosition = bestLocationSamplePoint2(Capsule1, Capsule2, m_SamplePoints(m_SamplePointsInBones.at(0).at(m)), frame);
						//Eigen::Vector3f newPosition = bestLocationSamplePoint3(Capsule2, m_SamplePoints(m_SamplePointsInBones.at(0).at(m)), TriIDs, frame);

						U_bc.row(m_SamplePointsInBones.at(0).at(m)) = Eigen::RowVector3f(newPosition);
					}

				}
			}
			for (int i = 0; i < affectedTriangles.size(); i++) {
				for (int v = 0; v < 3; v++) {
					int32_t VertexID = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).y()).Vertices[v];
					int32_t m = 0;
					bool found = false;
					while (found == false && m < m_SamplePointsInBones.at(1).size()) {
						int32_t samplePointID = m_SamplePoints(m_SamplePointsInBones.at(1).at(m));
						if (VertexID == samplePointID)
							found = true;
						else {
							m++;
						}
					}
					if (found == true) {
						//int32_t TriIDs[3];
						//TriIDs[0] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[0];
						//TriIDs[1] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[1];
						//TriIDs[2] = m_pMesh->getBoneSubmesh(0)->Faces.at(affectedTriangles.at(i).x()).Vertices[2];

						//T3DMesh<float>::Line Capsule2 = m_pMesh->getCapsuleDirection(boneMesh2, AnimationID, frame);
						T3DMesh<float>::Line Capsule1 = m_pMesh->getCapsuleDirection(boneMesh1, AnimationID, frame);

						float radius1 = 14.0f;
						//float radius2 = boneMesh2->capsuleRadius;
						//Eigen::Vector2f optimalRadius = getOptimalCapsuleRadius(Capsule1, radius1, Capsule2, radius2);
						Eigen::Vector2f optimalRadius;
						optimalRadius(0) = 9.0f;
						optimalRadius(1) = 9.0f;
						float accurateRadius = getAccurateCapsuleRadius(Capsule1, Submesh1Index - 1, m_SamplePoints(m_SamplePointsInBones.at(1).at(m)), frame);
						printf("Radius: %d\n", accurateRadius);
						Eigen::Vector3f newPosition = bestLocationSamplePoint(Capsule1, accurateRadius, m_SamplePoints(m_SamplePointsInBones.at(1).at(m)), frame);

						//Eigen::Vector3f newPosition = bestLocationSamplePoint(Capsule1, optimalRadius(0), m_SamplePoints(m_SamplePointsInBones.at(1).at(m)), frame);
						//Eigen::Vector3f newPosition = bestLocationSamplePoint2(Capsule1, Capsule2, m_SamplePoints(m_SamplePointsInBones.at(1).at(m)), frame);
						//Eigen::Vector3f newPosition = bestLocationSamplePoint3(Capsule1, m_SamplePoints(m_SamplePointsInBones.at(1).at(m)), TriIDs, frame);

						U_bc.row(m_SamplePointsInBones.at(1).at(m)) = Eigen::RowVector3f(newPosition);
					}

				}
			}
			*/

			std::vector<std::vector<int32_t>> affectedControlPoints = findAffectedControlPoints(Submesh1, Submesh2, checkMesh1, checkMesh2, affectedTriangles, frame);
			T3DMesh<float>::Line Capsule1 = m_pMesh->getCapsuleDirectionOld(boneMesh1, AnimationID, frame);
			T3DMesh<float>::Line Capsule2 = m_pMesh->getCapsuleDirectionOld(boneMesh2, AnimationID, frame);
			
			printf("Mesh1, Capsule Point %.2f , %.2f, %.2f, Dir  %.2f , %.2f, %.2f\n", 
				Capsule1.p.x(), Capsule1.p.y(), Capsule1.p.z(), Capsule1.direction.x(), Capsule1.direction.y(), Capsule1.direction.z());
			for (int i = 0; i < affectedControlPoints.at(0).size(); i++) {
				int32_t samplePointID = affectedControlPoints.at(0).at(i);
				int32_t vertexID = this->m_SamplePoints(samplePointID);
				Eigen::Vector3f VertexPosition = m_pMesh->getUpdatedVertexPositionOld(frame, vertexID);
				printf("Vertex %d has Pos %.2f, %.2f, %.2f\n", vertexID, VertexPosition.x(), VertexPosition.y(), VertexPosition.z());
				Eigen::Vector3f closestVert = getcloseVert2(Capsule2, Submesh2Index - 1, VertexPosition, frame);
				printf("ClosestVertex has Pos %.2f, %.2f, %.2f\n", closestVert.x(), closestVert.y(), closestVert.z());
				Eigen::Vector3f center = m_pMesh->closestPointOnLineSegment(Capsule2.p, Capsule2.p + Capsule2.direction, closestVert);
				float accurateRadius = m_pMesh->vectorLength(closestVert - center);

				printf("%.2f\n", accurateRadius);

				Eigen::Vector3f newPosition;
				if (method == 1) {
					newPosition = bestLocationSamplePoint(Capsule2, accurateRadius, VertexPosition, frame);
				}
				else if (method == 2) {
					newPosition = bestLocationSamplePoint2(Capsule1, Capsule2, accurateRadius, VertexPosition, frame);
				}
				else if (method == 4) {
					newPosition = bestLocationSamplePoint4(Capsule1, Capsule2, accurateRadius, VertexPosition, frame);
				}
				else {
					newPosition = bestLocationSamplePoint3(Capsule1, Capsule2, accurateRadius, VertexPosition, frame);
				}

				U_bc.row(samplePointID) = Eigen::RowVector3f(newPosition);
			}
			printf("Mesh2, Capsule Point %.2f , %.2f, %.2f, Dir  %.2f , %.2f, %.2f\n",
				Capsule2.p.x(), Capsule2.p.y(), Capsule2.p.z(), Capsule2.direction.x(), Capsule2.direction.y(), Capsule2.direction.z());
			for (int i = 0; i < affectedControlPoints.at(1).size(); i++) {
				int32_t samplePointID = affectedControlPoints.at(1).at(i);
				int32_t vertexID = this->m_SamplePoints(samplePointID);
				Eigen::Vector3f VertexPosition = m_pMesh->getUpdatedVertexPositionOld(frame, vertexID);
				printf("Vertex %d has Pos %.2f, %.2f, %.2f\n", vertexID, VertexPosition.x(), VertexPosition.y(), VertexPosition.z());
				Eigen::Vector3f closestVert = getcloseVert2(Capsule1, Submesh1Index - 1, VertexPosition, frame);
				printf("ClosestVertex has Pos %.2f, %.2f, %.2f\n", closestVert.x(), closestVert.y(), closestVert.z());
				Eigen::Vector3f center = m_pMesh->closestPointOnLineSegment(Capsule1.p, Capsule1.p + Capsule1.direction, closestVert);
				float accurateRadius = m_pMesh->vectorLength(closestVert - center);
				printf("%.2f\n", accurateRadius);
				Eigen::Vector3f newPosition;
				if (method == 1) {
					newPosition = bestLocationSamplePoint(Capsule1, accurateRadius, VertexPosition, frame);
				}
				else if (method == 2) {
					newPosition = bestLocationSamplePoint2(Capsule2, Capsule1, accurateRadius, VertexPosition, frame);
				}
				else if (method == 4) {
					newPosition = VertexPosition;
				}
				else {
					newPosition = bestLocationSamplePoint3(Capsule2, Capsule1, accurateRadius, VertexPosition, frame);
				}

				U_bc.row(samplePointID) = Eigen::RowVector3f(newPosition);
			}

			affectedControlPoints.at(0).clear();
			affectedControlPoints.at(1).clear();
			affectedControlPoints.clear();

		}

		affectedTriangles.clear();
		checkMesh1.clear();
		checkMesh2.clear();

		boneMesh1 = nullptr;
		boneMesh2 = nullptr;
		Submesh1 = nullptr;
		Submesh2 = nullptr;

		delete[] boneMesh1;
		delete[] boneMesh2;
		delete[] Submesh1;
		delete[] Submesh2;

		return U_bc;

	}

	Eigen::MatrixXf ShapeDeformer::ShapeDeformation(int32_t frame, Eigen::MatrixXf U_bc, Eigen::MatrixXf V) {
		Eigen::MatrixXf U, V_bc;
		Eigen::MatrixXf D;

		V_bc.resize(this->m_SamplePoints.rows(), 3);

		for (int bi = 0; bi < this->m_SamplePoints.rows(); bi++)
		{
			V_bc.row(bi) = V.row(m_SamplePoints(bi));
		}

		Eigen::MatrixXf D_bc = U_bc - V_bc;
		igl::harmonic(V, this->mat_Faces, this->m_SamplePoints, D_bc, 2., D);
		return (V + D);
		//this->Tri_Deformations.at(frame) = V;
	}

	void ShapeDeformer::resolveCollisionsShapeDeformation(int Submesh1Index, int Submesh2Index, int32_t startFrame, int32_t endFrame, int32_t AnimationID, int32_t method) {
		Eigen::Vector2i bones;
		bones(0) = Submesh1Index - 1;
		bones(1) = Submesh2Index - 1;

		this->setSamplePoints();
		this->sortSamplePoints(bones);
		this->setFaceMatrix();
		//m_pMesh->setSkinningMats(startFrame, endFrame, AnimationID, m_pMesh->rootBone()->ID);

		m_pMesh->setSkinningMats(startFrame, endFrame, AnimationID, m_pMesh->rootBone()->ID);
		m_pMesh->setTri_Deformation(startFrame, endFrame, AnimationID);

		T3DMesh<float>::Bone* boneMesh1 = m_pMesh->getBone(Submesh1Index - 1);
		T3DMesh<float>::Bone* boneMesh2 = m_pMesh->getBone(Submesh2Index - 1);

		for (int32_t i = startFrame; i <= endFrame; i++) {
			m_pMesh->updateCurrentTri_Deformation(i, AnimationID);
			printf("Frame: %d\n", i);
			Eigen::MatrixXf U_bc;

			Eigen::MatrixXf MeshVertices = m_pMesh->getTriDeformationMatrix(i);

			//Eigen::MatrixXf MeshVertices(m_pMesh->vertexCount(), 3);
			//for (int j = 0; j < m_pMesh->vertexCount(); j++) {
			//	MeshVertices(j, 0) = m_pMesh->currentTri_Deformations(j * 3);
			//	MeshVertices(j, 1) = m_pMesh->currentTri_Deformations(j * 3+1);
			//	MeshVertices(j, 2) = m_pMesh->currentTri_Deformations(j * 3+2);
			//}

			U_bc.resize(this->m_SamplePoints.rows(), 3);
			for (int32_t j = 0; j < m_SamplePoints.rows(); j++) {
				U_bc.row(j) = Eigen::RowVector3f(MeshVertices(m_SamplePoints(j), 0), MeshVertices(m_SamplePoints(j), 1), MeshVertices(m_SamplePoints(j), 2));
			}
			U_bc = collisionTestShapeDeformation(Submesh1Index, Submesh2Index, i, AnimationID, method, U_bc, MeshVertices);
			Eigen::MatrixXf newV = ShapeDeformation(i, U_bc, MeshVertices);
			//igl::writeOBJ("TestOriginal.obj", MeshVertices, mat_Faces);
			std::string objName = "MuscleMan" + std::to_string(i) + ".obj";
			std::string objName2 = "OriginalMuscleMan" + std::to_string(i) + ".obj";
			igl::writeOBJ(objName, newV, mat_Faces);
			igl::writeOBJ(objName2, MeshVertices, mat_Faces);

			// Export with CForge
			/*T3DMesh<float> M;
			std::vector<Vector3f> Vertices;
			std::vector<T3DMesh<float>::Face> Faces;
			T3DMesh<float>::Submesh* pSub = new T3DMesh<float>::Submesh();
			pSub->Material = 0;

			for (uint32_t i = 0; i < newV.rows(); i++) {
				Vector3f v = Vector3f(newV(i, 0), newV(i, 1), newV(i, 2));
				Vertices.push_back(v);
			}

			for (uint32_t i = 0; i < mat_Faces.rows(); ++i) {
				T3DMesh<float>::Face F;
				F.Vertices[0] = mat_Faces(i, 0);
				F.Vertices[1] = mat_Faces(i, 1);
				F.Vertices[2] = mat_Faces(i, 2);
				pSub->Faces.push_back(F);
			}

			T3DMesh<float>::Material Mat;
			Mat.Color = Vector4f(0.7f, 0.7f, 0.7f, 1.0f);
			Mat.ID = 0;
			Mat.Metallic = 0.04f;
			Mat.Roughness = 0.1f
				;
			M.addMaterial(&Mat, true);
			M.addSubmesh(pSub, false);
			M.vertices(&Vertices);
			AssetIO::store(objName, &M);*/

		}

	}


}//names space