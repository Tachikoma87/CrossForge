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
            


            switch (treeType) {
                case Normal:
                    leavesGeometry.uvs.push_back(Vector2f(0, 1));
                    leavesGeometry.uvs.push_back(Vector2f(0, 0));
                    leavesGeometry.uvs.push_back(Vector2f(1, 0));
                    leavesGeometry.uvs.push_back(Vector2f(1, 1));

                    branchProperties.botRadius = 0.3;
                    branchProperties.topRadius = 0.2;
                    branchProperties.length = 5;
                    branchProperties.angleVariationStrength = PI / 55;
                    generateTree(geometry, 4, 3, 5, rotPos, branchProperties, leavesGeometry);
                    break;
                case Aspen:
                    branchProperties.botRadius = 0.15;
                    branchProperties.topRadius = 0.05;
                    branchProperties.length = 8;
                    branchProperties.angleVariationStrength = PI / 64;
                    generateAspenBirkTree(geometry, 8, 9999, rotPos, branchProperties, leavesGeometry);
                    break;
                case Palm:
                    branchProperties.botRadius = 0.25;
                    branchProperties.topRadius = 0.15;
                    branchProperties.length = 8;
                    branchProperties.angleVariationStrength = PI / 20;
                    generatePalmTree(geometry, 10, rotPos, branchProperties, leavesGeometry);
                    break;
                case Needle:
                    leavesGeometry.uvs.push_back(Vector2f(0, 1));
                    leavesGeometry.uvs.push_back(Vector2f(0, 0));
                    leavesGeometry.uvs.push_back(Vector2f(0.5f, 0));
                    leavesGeometry.uvs.push_back(Vector2f(0.5f, 1));
                    leavesGeometry.uvs.push_back(Vector2f(1, 0));
                    leavesGeometry.uvs.push_back(Vector2f(1, 1));

                    branchProperties.botRadius = 0.2;
                    branchProperties.topRadius = 0.00;
                    branchProperties.length = 10;
                    branchProperties.angleVariationStrength = PI / 100;
                    generateNeedleTree(geometry, 8, randomI(16, 24), randomI(4, 6), rotPos, branchProperties, leavesGeometry);
                    break;
            }

            ObjExporter::meshExport(geometry, exportPath + to_string(i) + ".obj");
            ObjExporter::meshExport(leavesGeometry, exportPath + "Leaves" + to_string(i) + ".obj");
        }
    }

    void TreeGenerator::generateTree(GEOMETRY &geometry, int recursionDepth, float numOfSplits, int complexity,
                                     RotPos rotPos,
                                     BranchProperties branchProperties, GEOMETRY& leavesGeometry) {
        if (recursionDepth <= 0) {
            generateLeavesQuad(leavesGeometry, rotPos, 1 * 1.5, 1383.0f / 1600.0f * 1.5, 0.1);
            rotPos.rotation.y() += PI / 2;
            generateLeavesQuad(leavesGeometry, rotPos, 1 * 1.5, 1383.0f / 1600.0f * 1.5, 0.1);
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
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1);
            extraBranchRotPos.rotation.y() += PI / 2;
            generateLeavesQuad(leavesGeometry, rotPos, 1, 1383.0f / 1600.0f, 0.1);
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

    void TreeGenerator::generatePalmTree(GEOMETRY& geometry, int complexity, RotPos rotPos,
                                         BranchProperties branchProperties, GEOMETRY& leavesGeometry) {
        rotPos = generateBranch(geometry, rotPos, complexity, complexity, branchProperties);

        generatePalmTop(leavesGeometry, rotPos, 8, 5);
    }

    void TreeGenerator::generateNeedleTree(GEOMETRY& geometry, int complexity, int circles, int numBranches, RotPos rotPos,
                                           BranchProperties branchProperties, GEOMETRY& leavesGeometry) {
        rotPos = generateBranch(geometry, rotPos, circles, complexity, branchProperties);

        for (int i = 2; i < circles - 1; i++) {
            int index = i * (complexity + 1);
            float scale = 1 - (float)i / circles;

            rotPos.position = geometry.vertices[index] - geometry.normals[index] * branchProperties.botRadius * scale;
            float angle = randomF(0, 360);

            
            scale *= 2;
            float height = rotPos.position.y();
            for (int j = 0; j < numBranches; j++) {
                

                angle += 360 / numBranches;
                
                rotPos.rotation = AngleAxisf(CForge::GraphicsUtility::degToRad(0), Vector3f::UnitX())
                                * AngleAxisf(CForge::GraphicsUtility::degToRad(angle), Vector3f::UnitY())
                                * AngleAxisf(CForge::GraphicsUtility::degToRad(400), Vector3f::UnitZ());
               
                rotPos.rotation.normalize();
                rotPos.position.y() = height + randomF(-0.3, 0.3);
                
                generateNeedleFoliage(leavesGeometry, rotPos, 1.2 * scale, 0.4 * scale, 1.8 * scale, 0);
            }
        }
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
        Matrix3f rotationMatrix = rotPos.rotation.toRotationMatrix();

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
                                          Eigen::Matrix3f &rotationMatrix, float uvCordU) {
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


    void TreeGenerator::generateLeavesQuad(GEOMETRY &geometry, RotPos &rotPos, float width, float height, float offset) {
        width /= 2;
        rotPos.rotation.normalize();
        Matrix3f rotMat = rotPos.rotation.toRotationMatrix();
        geometry.vertices.push_back((rotMat * Vector3f(-width, height - offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(-width, -offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(width, -offset, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(width, height - offset, 0)) + rotPos.position);

        geometry.normals.push_back(rotMat * Vector3f(0, 0, 1));
        int vertexIndex = geometry.vertices.size();
        int normalIndex = geometry.normals.size();
        geometry.faces.push_back({Vector3i(vertexIndex - 3, 1, normalIndex), Vector3i(vertexIndex - 2, 2, normalIndex), Vector3i(vertexIndex - 1, 3, normalIndex), Vector3i(vertexIndex, 4, normalIndex)});
    }

    void TreeGenerator::generateNeedleFoliage(GEOMETRY& geometry, RotPos& rotPos, float width, float height, float length, float offset) {
        width /= 2;
        rotPos.rotation.normalize();
        Matrix3f rotMat = rotPos.rotation.toRotationMatrix();
        geometry.vertices.push_back((rotMat * Vector3f(offset + length, 0, -width)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(offset, 0, -width)) + rotPos.position);
        
        geometry.vertices.push_back((rotMat * Vector3f(offset, height, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(offset + length, height, 0)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(offset, 0, width)) + rotPos.position);
        geometry.vertices.push_back((rotMat * Vector3f(offset + length, 0, width)) + rotPos.position);
        
        
        geometry.normals.push_back(rotMat * Vector3f(0, 0, 1)); // placeholder value


        int vertexIndex = geometry.vertices.size();
        int normalIndex = geometry.normals.size();
        geometry.faces.push_back({ Vector3i(vertexIndex - 5, 1, normalIndex), Vector3i(vertexIndex - 4, 2, normalIndex), Vector3i(vertexIndex - 3, 3, normalIndex), Vector3i(vertexIndex - 2, 4, normalIndex) });
        geometry.faces.push_back({ Vector3i(vertexIndex - 2, 4, normalIndex), Vector3i(vertexIndex - 3, 3, normalIndex), Vector3i(vertexIndex - 1, 5, normalIndex), Vector3i(vertexIndex, 6, normalIndex) });
    }

    void TreeGenerator::generatePalmTop(GEOMETRY& geometry, RotPos& rotPos, int numLeaves, float size) {
        rotPos.rotation.normalize();
        
        Quaternionf rotOrigin = rotPos.rotation;
        for (int j = 0; j < numLeaves / 2; j++) {
            float randAngle = randomF(0, 360.0 / numLeaves);
            for (int i = 0; i < numLeaves; i++) {
                Quaternionf rot = AngleAxisf(CForge::GraphicsUtility::degToRad(0), Vector3f::UnitX())
                    * AngleAxisf(CForge::GraphicsUtility::degToRad(360 * i / numLeaves + randAngle), Vector3f::UnitY())
                    * AngleAxisf(CForge::GraphicsUtility::degToRad(j * 80.0 / (numLeaves / 2)), Vector3f::UnitZ());
                rotPos.rotation = rotOrigin * rot;
                generatePalmLeave(geometry, rotPos, PI / 2, 3, 7, PI / 2, 1, 3);
            }
        }
        

    }

    void TreeGenerator::generatePalmLeave(GEOMETRY& geometry, RotPos& rotPos, float curveAngle, float radius, int numEdges, float innerAngle, float innerRadius, int innerEdgeNum) {
        Matrix3f rotMat = rotPos.rotation.toRotationMatrix();
        for (int i = 0; i < numEdges; i++) {
            float iAngle = curveAngle * i / numEdges;
            for (int j = 0; j < innerEdgeNum; j++) {
                float jAngle = innerAngle * j / (innerEdgeNum - 1) - innerAngle / 2;
                Vector3f pos = Vector3f(sin(iAngle) * radius, cos(iAngle) * radius + cos(jAngle) * innerRadius - radius - innerRadius, sin(jAngle) * innerRadius);

                geometry.vertices.push_back((rotMat * pos) + rotPos.position);
                geometry.normals.push_back(rotMat * Vector3f(0, 0, 1)); // placeholder value
                geometry.uvs.push_back(Vector2f((float) j / (innerEdgeNum - 1), (float) i/ (numEdges - 1)));
                if (i > 0 && j > 0) {
                    int vertexIndex = geometry.vertices.size();
                    geometry.faces.push_back({ Vector3i(vertexIndex, vertexIndex, vertexIndex), Vector3i(vertexIndex - innerEdgeNum, vertexIndex - innerEdgeNum, vertexIndex), Vector3i(vertexIndex - innerEdgeNum - 1, vertexIndex - innerEdgeNum - 1, vertexIndex), Vector3i(vertexIndex - 1, vertexIndex - 1, vertexIndex) });
                }
            }
        }
    }

    float TreeGenerator::randomF(float min, float max) {
        return (min + (float) rand() / (float) (RAND_MAX) * (max - min));
    }

    int TreeGenerator::randomI(int min, int max) {
        return min + (rand() % (max - min + 1));
    }
}