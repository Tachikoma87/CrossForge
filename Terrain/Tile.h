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

        explicit Tile(uint32_t sideLength, GLTexture2D* heightMap);
        ~Tile();

        void init();

        void render(RenderDevice* renderDevice, TileVariant variant);

        uint32_t getSideLength() const;

    private:
        vector<GLfloat> calculateVertices(uint32_t width, uint32_t height) const;
        vector<GLuint> calculateIndices(uint32_t width, uint32_t height, TileVariant variant) const;
        static void addTriangle(vector<GLuint>* indices, uint32_t a, uint32_t b, uint32_t c);

        void initBuffers(vector<GLfloat> *vertices, vector<GLuint> *indices, TileVariant variant);
        void initVertexArray(GLBuffer *vertexBuffer, GLBuffer *indexBuffer, TileVariant variant);
        void initTiles();
        void initLine();
        void initTrim();
        void initShader();

        uint32_t mSideLength;

        GLsizei mIndexBufferSizes[5];
        GLVertexArray mVertexArrays[5];

        GLTexture2D* mHeightMap;
        GLShader* mShader;
    };
}

#endif
