#ifndef TERRAIN_TILE_H
#define TERRAIN_TILE_H

#include <Eigen/Core>
#include <glad/glad.h>
#include <vector>
#include <CForge/Graphics/GLVertexArray.h>
#include "CForge/Graphics/GLBuffer.h"

using namespace CForge;
using namespace std;

namespace Terrain {
    class Tile {
    public:
        enum TileVariant {
            Normal = 0,
            Edge = 1,
            Corner = 2,
            Line = 3,
            Trim = 4,
        };

        explicit Tile(uint sideLength, GLTexture2D* heightMap);
        ~Tile();

        void init();

        void render(RenderDevice* renderDevice, TileVariant variant);

        uint getSideLength() const;

    private:
        vector<GLfloat> calculateVertices(uint width, uint height) const;
        vector<GLuint> calculateIndices(uint width, uint height, TileVariant variant) const;
        static void addTriangle(vector<GLuint>* indices, uint a, uint b, uint c);

        void initBuffers(vector<GLfloat> *vertices, vector<GLuint> *indices, TileVariant variant);
        void initVertexArray(GLBuffer *vertexBuffer, GLBuffer *indexBuffer, TileVariant variant);
        void initTileVertexArrays();
        void initLineVertexArray();
        void initShader();

        uint mSideLength;

        GLsizei mIndexBufferSizes[5];
        GLVertexArray mVertexArrays[5];

        GLTexture2D* mHeightMap;
        GLShader* mShader;
    };
}

#endif
