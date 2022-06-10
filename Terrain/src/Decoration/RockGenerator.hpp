#pragma once

#include "CForge/Core/CoreUtility.hpp"
#include "ObjExporter.hpp"
#include "CellularNoise3D.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "PerlinNoise.hpp"
#include <algorithm>


#define PI 3.14159265

using namespace Eigen;
using namespace std;

namespace Terrain {
    class RockGenerator {
    public:
        enum RockType {
            LowPoly,
            Normal,
            HighPoly
        };

        static void generateRocks(RockType treeType, int numberRocks, string exportPath);

    private:
        // generate stone
        static void generateRock(GEOMETRY &geometry, int complexity);

        // generate sphere
        static void generateSphere(GEOMETRY &geometry, int complexity);

        static float randomF(float min, float max);
    };
}
