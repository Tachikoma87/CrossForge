#include "TreeGenerator.hpp"

namespace Terrain {
    void TreeGenerator::generateTrees(TreeType treeType, int numberTrees, string exportPath) {
        for (int i = 0; i < numberTrees; i++) {
            // new random Seed
            srand(time(NULL));

            // tree generation
            GEOMETRY geometry;
            BranchProperties branchProperties;
            RotPos rotPos;
            rotPos.rotation = Quaternionf(1, 0, 0, 0);
            rotPos.position = Vector3f(0, 0, 0);

            GEOMETRY leavesGeometry;
            leavesGeometry.uvs.push_back(Vector2f(0, 1));
            leavesGeometry.uvs.push_back(Vector2f(0, 0));
            leavesGeometry.uvs.push_back(Vector2f(1, 0));
            leavesGeometry.uvs.push_back(Vector2f(1, 1));

            switch (treeType) {
                case Normal:
                    branchProperties.botRadius = 0.3;
                    branchProperties.topRadius = 0.2;
                    branchProperties.length = 5;
                    branchProperties.angleVariationStrength = PI / 55;
                    generateTree(geometry, 4, 3, 3, rotPos, branchProperties, leavesGeometry);
                    break;
                case Aspen:
                    branchProperties.botRadius = 0.15;
                    branchProperties.topRadius = 0.05;
                    branchProperties.length = 8;
                    branchProperties.angleVariationStrength = PI / 64;
                    generateAspenBirkTree(geometry, 8, 9999, rotPos, branchProperties, leavesGeometry);
                    break;
            }

            ObjExporter::meshExport(geometry, exportPath + "tree" + to_string(i) + ".obj");
            ObjExporter::meshExport(leavesGeometry, exportPath + "leaves" + to_string(i) + ".obj");
        }
    }

    void TreeGenerator::generateTree(GEOMETRY &geometry, int recursionDepth, float numOfSplits, int complexity,
                                     RotPos rotPos,
                                     BranchProperties branchProperties, GEOMETRY& leavesGeometry) {
        if (recursionDepth <= 0) {
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1, 3);
            rotPos.rotation.y() += PI / 2;
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1, 3);
            return;
        }

        numOfSplits -= 0.75;
        branchProperties.botRadius = branchProperties.topRadius;
        branchProperties.topRadius -= branchProperties.topRadius / recursionDepth;
        branchProperties.length *= 0.7;
        branchProperties.angleVariationStrength *= 2;

        rotPos = generateBranch(geometry, rotPos, complexity, complexity, branchProperties);

        // steep angle branches
        RotPos extraBranchRotPos;
        BranchProperties extraBranchProperties;
        extraBranchProperties.topRadius = branchProperties.topRadius * 0.8;
        extraBranchProperties.length = branchProperties.length * 0.8;
        extraBranchProperties.angleVariationStrength = branchProperties.angleVariationStrength;
        std::vector<int> extraBranchIndices;
        for (int i = 0; i < numOfSplits; i++) {
            extraBranchIndices.push_back(
                randomI(geometry.vertices.size() - (complexity * complexity * 2 / 3),
                        geometry.vertices.size() - 1));
            int index = randomI(geometry.vertices.size() - (complexity * complexity * 2 / 3),
                                geometry.vertices.size() - 1);
            extraBranchRotPos.position = geometry.vertices[index] - geometry.normals[index] * branchProperties.topRadius;
            extraBranchRotPos.rotation = Quaternionf().setFromTwoVectors(Vector3f(0, 1, 0), geometry.normals[index]);
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1, 3);
            extraBranchRotPos.rotation.y() += PI / 2;
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1, 3);
        }
        for (int i = 0; i < numOfSplits; i++) {
            
            extraBranchRotPos.position = geometry.vertices[extraBranchIndices[i]] -
                                         geometry.normals[extraBranchIndices[i]] * branchProperties.topRadius;
            extraBranchRotPos.rotation = Quaternionf().setFromTwoVectors(Vector3f(0, 1, 0),
                                                                         geometry.normals[extraBranchIndices[i]]);
            extraBranchRotPos.rotation.x() *= 0.6;
            extraBranchRotPos.rotation.z() *= 0.6;
            extraBranchRotPos.rotation.normalize();
            generateTree(geometry, recursionDepth - 1, numOfSplits, complexity, extraBranchRotPos,
                         extraBranchProperties, leavesGeometry);
        }

        // smooth branches
        for (int i = 0; i < numOfSplits + 0.5; i++) {
            generateTree(geometry, recursionDepth - 1, numOfSplits, complexity, rotPos, branchProperties, leavesGeometry);
        }
    }

    void TreeGenerator::generateAspenBirkTree(GEOMETRY &geometry, int complexity, int numBranches, RotPos rotPos,
                                              BranchProperties branchProperties, GEOMETRY& leavesGeometry) {
        // main trunk
        int numRings = complexity * branchProperties.length / 2;
        rotPos = generateBranch(geometry, rotPos, numRings, complexity, branchProperties);

        numBranches = min(numBranches, numRings);
        std::vector<int> extraBranchIndices;
        for (float i = numRings / 3; i < numRings + 1; i += numRings / (float) numBranches) {
            extraBranchIndices.push_back(randomI((i - 1) * complexity, i * complexity));
        }
        branchProperties.angleVariationStrength *= 4;
        branchProperties.botRadius /= 2;
        branchProperties.topRadius /= 2;
        branchProperties.length /= 8;
        for (int i = 0; i < extraBranchIndices.size(); i++) {
            RotPos extraBranchRotPos;
            extraBranchRotPos.position = geometry.vertices[extraBranchIndices[i]] -
                                         geometry.normals[extraBranchIndices[i]] * branchProperties.topRadius;
            extraBranchRotPos.rotation = Quaternionf().setFromTwoVectors(Vector3f(0, 1, 0),
                                                                         geometry.normals[extraBranchIndices[i]]);
            extraBranchRotPos.rotation.x() *= 0.6;
            extraBranchRotPos.rotation.z() *= 0.6;
            extraBranchRotPos.rotation.normalize();
            generateTree(geometry, 3, 2, 3, extraBranchRotPos, branchProperties, leavesGeometry);
        }
        /*
        branchProperties.topRadius *= 2;
        branchProperties.length /= 2;
        generateTree(geometry, 3, 2, complexity, rotPos, branchProperties);
        */
    }

    // generate distorted cylinder with radius interpolation
    TreeGenerator::RotPos
    TreeGenerator::generateBranch(GEOMETRY &geometry, RotPos rotPos, int numCircles, int pointsPerCirlce,
                                  BranchProperties branchProperties) {
        float stepDistance = branchProperties.length / numCircles;
        float angleStep = branchProperties.angleVariationStrength / numCircles;
        float radiusStep = (branchProperties.topRadius - branchProperties.botRadius) / numCircles;
        float radius = branchProperties.botRadius;
        float uvStepVertically =
            stepDistance /
            (2 * PI * radius); // Textur Verzerrung vermeiden indem der Kreisumfang auf 1 normiert wird
        float uvCordU = 0;
        Vector2f rotationAcceleration = Vector2f(0, 0);
        Matrix3<float> rotationMatrix = rotPos.rotation.toRotationMatrix();

        generateCircleVertices(geometry, pointsPerCirlce, rotPos.position, radius, rotationMatrix, uvCordU);
        for (int i = 1; i <= numCircles; i++) {
            radius += radiusStep;
            float uvStepVertically = stepDistance / (2 * PI * radius);
            uvCordU += uvStepVertically;
            rotationAcceleration.x() += randomF(-angleStep, angleStep);
            rotationAcceleration.y() += randomF(-angleStep, angleStep);
            rotPos.rotation.x() = (rotPos.rotation.x() + rotationAcceleration.x()) * 0.95;
            rotPos.rotation.z() = (rotPos.rotation.z() + rotationAcceleration.y()) * 0.95;
            rotPos.rotation.normalize();
            rotationMatrix = rotPos.rotation.toRotationMatrix();
            rotPos.position += rotationMatrix * Vector3f(0, stepDistance, 0);
            generateCircleVertices(geometry, pointsPerCirlce, rotPos.position, radius, rotationMatrix, uvCordU);
            // set faces
            for (int j = geometry.vertices.size(); j > geometry.vertices.size() - pointsPerCirlce; j--) {
                int faceIndex = geometry.faces.size();
                geometry.faces.push_back(std::vector<Vector3i>());
                int vertexIndex = j - 1;
                geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                vertexIndex -= pointsPerCirlce + 1;
                geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                vertexIndex++;
                geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                vertexIndex = j;
                geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
            }
        }
        return rotPos;
    }

    void
    TreeGenerator::generateCircleVertices(GEOMETRY &geometry, int pointsPerCircle, Vector3f centrePoint, float radius,
                                          Matrix3<float> &rotationMatrix, float uvCordU) {
        for (int i = 0; i <= pointsPerCircle; i++) {
            float angle = 2 * PI * ((float) i) / pointsPerCircle;
            Vector3f normal(cos(angle), 0, -1 * sin(angle));
            normal = rotationMatrix * normal;
            Vector3f verticePos = normal * radius + centrePoint;
            geometry.vertices.push_back(verticePos);
            geometry.uvs.push_back(Vector2f(((float) i) / pointsPerCircle, uvCordU));
            geometry.normals.push_back(normal);
        }
    }


    void TreeGenerator::generateLeavesQuad(GEOMETRY &geometry, RotPos &rotPos, float width, float height, float offset, int numFaces) {
        width /= 2;
        rotPos.rotation.normalize();
        Matrix3f rotMat = rotPos.rotation.toRotationMatrix();
        geometry.vertices.push_back((rotMat * Vector3f(-width, height - offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(-width, -offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(width, -offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(width, height - offset, 0)) + rotPos.position);

        geometry.uvs.push_back(Vector2f(0, 1));
        geometry.uvs.push_back(Vector2f(0, 0));
        geometry.uvs.push_back(Vector2f(1, 0));
        geometry.uvs.push_back(Vector2f(1, 1));

        geometry.normals.push_back(rotMat * Vector3f(0, 0, 1));
        int vertexIndex = geometry.vertices.size();
        int normalIndex = geometry.normals.size();
        geometry.faces.push_back({Vector3i(vertexIndex - 3, 1, normalIndex), Vector3i(vertexIndex - 2, 2, normalIndex), Vector3i(vertexIndex - 1, 3, normalIndex), Vector3i(vertexIndex, 4, normalIndex)});
    }


    float TreeGenerator::randomF(float min, float max) {
        return (min + (float) rand() / (float) (RAND_MAX) * (max - min));
    }

    int TreeGenerator::randomI(int min, int max) {
        return min + (rand() % (max - min + 1));
    }
}