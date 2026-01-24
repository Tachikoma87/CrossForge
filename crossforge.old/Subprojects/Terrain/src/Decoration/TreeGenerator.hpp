#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include "CForge/Graphics/GraphicsUtility.h"
#include "ObjExporter.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>

#define PI 3.14159265

using namespace Eigen;
using namespace std;

namespace Terrain {
    class TreeGenerator {
    public:
        enum TreeType {
            Normal, Aspen, Palm, Needle
        };

        static void generateTrees(TreeType treeType, int numberTrees, string exportPath);


    private:
        struct RotPos {
            Quaternionf rotation;
            Vector3f position;
        };

        struct BranchProperties {
            float topRadius;
            float botRadius;
            float length;
            float angleVariationStrength;
        };

        // generate "standard" tree
        static void generateTree(GEOMETRY &geometry, int recursionDepth, float numOfSplits, int complexity,
                                 RotPos rotPos, BranchProperties branchProperties, GEOMETRY& leavesGeometry);

        static void generateAspenBirkTree(GEOMETRY &geometry, int complexity, int numBranches, RotPos rotPos,
                                          BranchProperties branchProperties, GEOMETRY& leavesGeometry);

        static void generatePalmTree(GEOMETRY& geometry, int complexity, RotPos rotPos,
                                     BranchProperties branchProperties, GEOMETRY& leavesGeometry);

        static void generateNeedleTree(GEOMETRY& geometry, int complexity, int circles, int numBranches, RotPos rotPos,
                                       BranchProperties branchProperties, GEOMETRY& leavesGeometry);

        // generate distorted cylinder with radius interpolation
        static RotPos generateBranch(GEOMETRY &geometry, RotPos rotPos, int numCircles, int pointsPerCirlce,
                                     BranchProperties branchProperties);

        static void generateCircleVertices(GEOMETRY &geometry, int pointsPerCircle, Vector3f centrePoint, float radius,
                                           Eigen::Matrix3f &rotationMatrix, float uvCordU);

        static void generateLeavesQuad(GEOMETRY &geometry, RotPos &rotPos, float width, float height, float offset);

        static void generateNeedleFoliage(GEOMETRY& geometry, RotPos& rotPos, float width, float height, float length, float offset);

        static void generatePalmTop(GEOMETRY& geometry, RotPos& rotPos, int numLeaves, float size);

        static void generatePalmLeave(GEOMETRY& geometry, RotPos& rotPos, float curveAngle, float radius, int numEdges, float innerAngle, float innerRadius, int innerEdgeNum);
        
        static float randomF(float min, float max);

        static int randomI(int min, int max);
    };
}
