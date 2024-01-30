#pragma once

#include <CForge/Graphics/GLBuffer.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/OpenGLHeader.h>

using namespace CForge;
using namespace std;

namespace Terrain {
    class ClipMap {
    public:
        struct ClipMapConfig {
            /// The side length of normal tile. The vertex count is one more. This value must be a multiple of two.
            uint32_t sideLength;
            /// The number of different level of details the clipmap consists of.
            uint32_t levelCount;
        };

        static const uint32_t TILE_COUNT = 6;
        enum TileVariant {
            Normal = 0,
            Edge = 1,
            Corner = 2,
            Cross = 3,
            Line = 4,
            Trim = 5,
			count,
        };

        ClipMap();
        ~ClipMap();

        void generate(ClipMapConfig config);

        const ClipMapConfig &getConfig();

        GLsizei getIndexCount(TileVariant variant);
        void bindTile(TileVariant variant);
        void unbindTile(TileVariant variant);
    private:
        static void calculateVertices(vector<GLfloat>& vertices, uint32_t width, uint32_t height, float offsetX=0.0f, float offsetY=0.0f, bool swapPos=false);
        static void calculateIndices(vector<GLuint>& indices, uint32_t width, uint32_t height, TileVariant variant, uint32_t offset=0);
        static void addTriangle(vector<GLuint>& indices, uint32_t a, uint32_t b, uint32_t c);

        void clear();

        // the same vertex buffer is shared by Normal, Edge and Corner
        GLBuffer* initVertexBuffer(vector<GLfloat>& vertices);
        GLBuffer* initIndexBuffer(vector<GLuint>& indices);
        void initVertexArrays();

        void initTiles(uint32_t sideLength);
        void initCross(uint32_t sideLength);
        void initLine(uint32_t sideLength);
        void initTrim(uint32_t sideLength);

        ClipMapConfig mConfig;

        GLBuffer* mVertexBuffers[TILE_COUNT];
        GLBuffer* mIndexBuffers[TILE_COUNT];
        GLVertexArray mVertexArrays[TILE_COUNT];
    };
}
