#ifndef TERRAIN_TILE_H
#define TERRAIN_TILE_H

#include <glad/glad.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>

using namespace CForge;
using namespace std;

namespace Terrain {
    // Todo: rename to ClipMap
    class Tile {
    public:
        enum TileVariant {
            Normal = 0,
            Edge = 1,
            Corner = 2,
            Line = 3,
            Trim = 4,
            Cross = 5,
        };

        Tile(uint32_t sideLength, GLTexture2D* heightMap);

        void render(RenderDevice* renderDevice, TileVariant variant);
        uint32_t sideLength() const;

    private:
        static void calculateVertices(vector<GLfloat>& vertices, uint32_t width, uint32_t height, float offsetX=0.0f, float offsetY=0.0f, bool swapPos=false);
        static void calculateIndices(vector<GLuint>& indices, uint32_t width, uint32_t height, TileVariant variant, uint32_t offset=0);
        static void addTriangle(vector<GLuint>& indices, uint32_t a, uint32_t b, uint32_t c);

        void initBuffers(vector<GLfloat>& vertices, vector<GLuint>& indices, TileVariant variant);
        void initVertexArray(GLBuffer& vertexBuffer, GLBuffer& indexBuffer, TileVariant variant);
        void initTiles();
        void initLine();
        void initTrim();
        void initCross();
        void initShader();

        uint32_t mSideLength;

        GLsizei mIndexBufferSizes[6];
        GLVertexArray mVertexArrays[6];

        GLTexture2D* mHeightMap;
        GLShader* mShader;
    };
}

#endif
