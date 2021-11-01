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
        };

        explicit Tile(uint sideLength, GLTexture2D* heightMap);
        ~Tile();

        void init();

        void render(RenderDevice *renderDevice, TileVariant variant);

        uint getSideLength() const;

    private:
        vector<GLfloat> calculateVertices() const;
        vector<GLuint> calculateIndices(TileVariant variant) const;
        static void addTriangle(vector<GLuint>* indices, uint a, uint b, uint c);

        void initVertexArray(GLBuffer *vertexBuffer, GLBuffer *indexBuffer, TileVariant variant);
        void initShader();

        uint mSideLength;
        uint mVertexCount;

        GLsizei mIndexBufferSizes[3];
        GLVertexArray mVertexArrays[3];

        GLTexture2D* mHeightMap;
        GLShader* mShader;
    };
}

#endif
