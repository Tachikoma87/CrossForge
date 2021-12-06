#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include "CForge/AssetIO/ObjExporter.hpp"
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
            Normal, Aspen
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
                                 RotPos rotPos, BranchProperties branchProperties);

        static void generateAspenBirkTree(GEOMETRY &geometry, int complexity, int numBranches, RotPos rotPos,
                                          BranchProperties branchProperties);

        // generate distorted cylinder with radius interpolation
        static RotPos generateBranch(GEOMETRY &geometry, RotPos rotPos, int numCircles, int pointsPerCirlce,
                                     BranchProperties branchProperties);

        static void generateCircleVertices(GEOMETRY &geometry, int pointsPerCircle, Vector3f centrePoint, float radius,
                                           Matrix3<float> &rotationMatrix, float uvCordU);

        static void generateTestQuad(GEOMETRY &geometry);

        static float randomF(float min, float max);

        static int randomI(int min, int max);
    };
}
