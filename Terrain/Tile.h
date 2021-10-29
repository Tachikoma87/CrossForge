#ifndef TERRAIN_TILE_H
#define TERRAIN_TILE_H

#include <Eigen/Core>
#include <glad/glad.h>
#include <vector>

using namespace Eigen;
using namespace std;

namespace Terrain {
    class Tile {
    public:
        enum TileVariant {
            Normal,
            Edge,
            Corner,
        };

        Tile(uint sideLength);
        ~Tile();

        void calculateIndices(TileVariant variant);


        GLuint* getIndices();
        GLfloat* getVertices();

        uint getIndexBufferSize();
        uint getVertexBufferSize();


    private:
        void addTriangle(uint a, uint b, uint c);

        uint mSideLength;
        uint mVertexCount;
        vector<GLfloat> mVertices;
        vector<GLuint> mIndices;
    };
}

#endif
