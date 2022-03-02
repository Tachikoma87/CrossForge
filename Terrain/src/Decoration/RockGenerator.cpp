#include "RockGenerator.hpp"

namespace Terrain {
    void RockGenerator::generateRocks(RockType rockType, int numberRocks, string exportPath) {
        for (int i = 0; i < numberRocks; i++) {
            

            // new random Seed
            srand(time(NULL));

            // rock generation
            GEOMETRY geometry;
          
            switch (rockType) {
                case LowPoly:
                    generateRock(geometry, 16);
                    break;
                case Normal:
                    generateRock(geometry, 32);
                    break;
                case HighPoly:
                    generateRock(geometry, 64);
                    break;
            }

            ObjExporter::meshExport(geometry, exportPath + to_string(i) + ".obj");
        }
    }

    void RockGenerator::generateRock(GEOMETRY &geometry, int complexity) {
        generateSphere(geometry, complexity);

        // generate noise
        CellularNoise3D cNoise(10, 1, 1, 1);
        siv::PerlinNoise pNoise;
        
        // move vertices
        for (int i = 0; i < geometry.vertices.size(); i++) {
            geometry.vertices[i] += geometry.normals[i] * cNoise.getValue(geometry.vertices[i], 0, 4);
            geometry.vertices[i] *= pNoise.accumulatedOctaveNoise3D(geometry.vertices[i].x(), geometry.vertices[i].y(), geometry.vertices[i].z(), 8) * 0.1 + 0.9;
        }
    }

    void RockGenerator::generateSphere(GEOMETRY &geometry, int complexity) {

        float radius = 1;

        float uvStepU = 1.0f / complexity;
        float uvStepV = 1.0f / (complexity - 1);

        float vertexIndex = 0;
        for (int latitude = 0; latitude < complexity; latitude++) {
            for (int longitude = 0; longitude < complexity + 1; longitude++) {
                geometry.uvs.push_back(Vector2f(longitude * uvStepU, latitude * uvStepV));

                float longitudeAngle = geometry.uvs[vertexIndex].x() * 2 * PI;
                float latitudeAngle = (geometry.uvs[vertexIndex].y() - 0.5f) * PI;

                float ringRadius = cos(latitudeAngle) + 0.0001f;

                Vector3f pos = Vector3f(ringRadius * cos(longitudeAngle), sin(latitudeAngle), -ringRadius * sin(longitudeAngle));
                geometry.vertices.push_back(pos);
                geometry.normals.push_back(pos);

                vertexIndex++;
            }

            if (latitude > 0) {
                for (int j = geometry.vertices.size(); j > geometry.vertices.size() - complexity; j--) {
                    int faceIndex = geometry.faces.size();
                    geometry.faces.push_back(std::vector<Vector3i>());
                    int vertexIndex = j - 1;
                    geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                    vertexIndex -= complexity + 1;
                    geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                    vertexIndex++;
                    geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                    vertexIndex = j;
                    geometry.faces[faceIndex].push_back(Vector3i(vertexIndex, vertexIndex, vertexIndex));
                }
            }
        }


    }

    float RockGenerator::randomF(float min, float max) {
        return (min + (float) rand() / (float) (RAND_MAX) * (max - min));
    }
}