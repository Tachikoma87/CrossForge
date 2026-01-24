#include <CForge/Graphics/RenderDevice.h>
#include "ClipMap.h"

namespace Terrain {

    ClipMap::ClipMap() : mVertexBuffers(), mIndexBuffers() {}

    ClipMap::~ClipMap() {
        clear();
    }

    void ClipMap::generate(ClipMapConfig config) {
        mConfig = config;

        clear();
        initTiles(config.sideLength);
        initCross(config.sideLength);
        initLine(config.sideLength);
        initTrim(config.sideLength * 2 + 1);
        initVertexArrays();
    }

    GLsizei ClipMap::getIndexCount(ClipMap::TileVariant variant) {
        return mIndexBuffers[variant]->size();
    }

    void ClipMap::bindTile(ClipMap::TileVariant variant) {
        mVertexArrays[variant].bind();
    }

    void ClipMap::unbindTile(ClipMap::TileVariant variant) {
        mVertexArrays[variant].unbind();
    }

    void ClipMap::calculateVertices(vector<GLfloat>& vertices, uint32_t width, uint32_t height, float offsetX, float offsetY, bool swapPos) {
        // width and height in triangle side count, vertex count one more
        for (uint32_t y = 0; y <= height; y++) {
            for (uint32_t x = 0; x <= width; x++) {
                float positionX = static_cast<float>(x) - 0.5f * static_cast<float>(width) + offsetX;
                float positionY = static_cast<float>(y) - 0.5f * static_cast<float>(height) + offsetY;

                if (swapPos) {
                    vertices.push_back(positionY);
                    vertices.push_back(positionX);
                }
                else {
                    vertices.push_back(positionX);
                    vertices.push_back(positionY);
                }
            }
        }
    }

    void ClipMap::calculateIndices(vector<GLuint>& indices, uint32_t width, uint32_t height, TileVariant variant, uint32_t offset) {
        uint32_t vertexCount = width + 1;

        // a---b---c
        // | \ | / |
        // d---e---f
        // | / | \ |
        // g---h---i
        for (uint32_t y = 0; y < height; y += 2) {
            for (uint32_t x = 0; x < width; x += 2) {
                auto a = y * vertexCount + x + offset;
                auto b = a + 1;
                auto c = a + 2;
                auto d = a + vertexCount;
                auto e = a + vertexCount + 1;
                auto f = a + vertexCount + 2;
                auto g = a + 2 * vertexCount;
                auto h = a + 2 * vertexCount + 1;
                auto i = a + 2 * vertexCount + 2;

                if (x == 0 && variant != Normal) {
                    addTriangle(indices, e, g, a);
                } else {
                    addTriangle(indices, e, g, d);
                    addTriangle(indices, e, d, a);
                }

                if (y == 0 && variant == Corner) {
                    addTriangle(indices, e, a, c);
                } else {
                    addTriangle(indices, e, a, b);
                    addTriangle(indices, e, b, c);
                }

                addTriangle(indices, e, c, f);
                addTriangle(indices, e, f, i);
                addTriangle(indices, e, i, h);
                addTriangle(indices, e, h, g);
            }
        }
    }

    void ClipMap::addTriangle(vector<GLuint>& indices, uint32_t a, uint32_t b, uint32_t c) {
        indices.push_back(a);
        indices.push_back(c);
        indices.push_back(b);
    }

    void ClipMap::clear() {
        for (auto vertexArray : mVertexArrays) {
            vertexArray.clear();
        }

        // free the vertex buffer shared by Normal, Edge and Corner only once
        for (uint32_t variant = Corner; variant < TILE_COUNT; variant++) {
            delete mVertexBuffers[variant];
        }

        for (auto indexBuffer : mIndexBuffers) {
            delete indexBuffer;
        }
    }

    GLBuffer *ClipMap::initVertexBuffer(vector<GLfloat> &vertices) {
        GLBuffer* vertexBuffer = new GLBuffer();
        vertexBuffer->init(GLBuffer::BTYPE_VERTEX,
                          GLBuffer::BUSAGE_STATIC_DRAW,
                          vertices.data(),
                          vertices.size() * sizeof(GLfloat));

        return vertexBuffer;
    }

    GLBuffer *ClipMap::initIndexBuffer(vector<GLuint>& indices) {
        GLBuffer* indexBuffer = new GLBuffer();
        indexBuffer->init(GLBuffer::BTYPE_INDEX,
                         GLBuffer::BUSAGE_STATIC_DRAW,
                         indices.data(),
                         indices.size() * sizeof(GLuint));

        return indexBuffer;
    }

    void ClipMap::initVertexArrays() {
        for (uint32_t variant = 0; variant < TILE_COUNT; variant++) {
            mVertexArrays[variant].init();
            mVertexArrays[variant].bind();
            mVertexBuffers[variant]->bind();
            mIndexBuffers[variant]->bind();

            glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
            glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION),
                                  2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

            mVertexArrays[variant].unbind();
        }
    }

    void ClipMap::initTiles(uint32_t sideLength) {
        vector<GLfloat> vertices;
        calculateVertices(vertices, sideLength, sideLength);

        GLBuffer* vertexBuffer = initVertexBuffer(vertices);

        for (uint32_t variant = Normal; variant <= Corner; variant++) {
            vector<GLuint> indices;
            calculateIndices(indices, sideLength, sideLength, static_cast<TileVariant>(variant));

            mVertexBuffers[variant] = vertexBuffer;
            mIndexBuffers[variant] = initIndexBuffer(indices);
        }
    }

    void ClipMap::initCross(uint32_t sideLength) {
        float positionOffset = sideLength / 2 + 1;
        uint32_t vertexOffset = 3 * (sideLength + 1);

        vector<GLfloat> vertices;
        vector<GLuint> indices;

        calculateVertices(vertices, 2, sideLength, 0, positionOffset);
        calculateVertices(vertices, 2, sideLength, 0, -positionOffset);
        calculateVertices(vertices, sideLength, 2, positionOffset);
        calculateVertices(vertices, sideLength, 2, -positionOffset);
        calculateVertices(vertices, 2, 2);

        calculateIndices(indices, 2,  sideLength, static_cast<TileVariant>(Normal));
        calculateIndices(indices, 2, sideLength, static_cast<TileVariant>(Normal), vertexOffset);
        calculateIndices(indices, sideLength, 2, static_cast<TileVariant>(Normal), 2 * vertexOffset);
        calculateIndices(indices, sideLength, 2, static_cast<TileVariant>(Normal), 3 * vertexOffset);
        calculateIndices(indices, 2, 2, static_cast<TileVariant>(Normal), 4 * vertexOffset);

        mVertexBuffers[Cross] = initVertexBuffer(vertices);
        mIndexBuffers[Cross] = initIndexBuffer(indices);
    }

    void ClipMap::initLine(uint32_t sideLength) {
        vector<GLfloat> vertices;
        vector<GLuint> indices;
        calculateVertices(vertices, sideLength, 2);
        calculateIndices(indices, sideLength, 2, static_cast<TileVariant>(Edge));

        mVertexBuffers[Line] = initVertexBuffer(vertices);
        mIndexBuffers[Line] = initIndexBuffer(indices);
    }

    void ClipMap::initTrim(uint32_t sideLength) {
        uint32_t cornerIndex = (sideLength + 1) * 2;
        float positionOffset = static_cast<float>(sideLength) / 2.0f;

        vector<GLfloat> vertices;
        vector<GLuint> indices;

        calculateVertices(vertices, 1, sideLength, -positionOffset - 0.5f);
        vertices.push_back(-positionOffset - 1.0f);
        vertices.push_back(-positionOffset - 1.0f);
        calculateVertices(vertices, 1, sideLength, -positionOffset - 0.5f, 0, true);

        addTriangle(indices, cornerIndex, 1, 0);
        addTriangle(indices, cornerIndex, cornerIndex + 1, cornerIndex + 2);

        for (uint32_t y = 0; y < sideLength; y++) {
            auto a = y * 2;
            auto b = a + 1;
            auto c = a + 2;
            auto d = a + 3;

            if (y % 2 == 0) {
                addTriangle(indices, a, b, c);
                addTriangle(indices, b, d, c);
            } else {
                addTriangle(indices, a, b, d);
                addTriangle(indices, a, d, c);
            }
        }

        for (uint32_t y = sideLength + 1; y < sideLength * 2 + 1; y++) {
            auto a = y * 2 + 1;
            auto b = a + 1;
            auto c = a + 2;
            auto d = a + 3;

            if (y % 2 == 0) {
                addTriangle(indices, a, c, b);
                addTriangle(indices, b, c, d);
            } else {
                addTriangle(indices, a, d, b);
                addTriangle(indices, a, c, d);
            }
        }

        mVertexBuffers[Trim] = initVertexBuffer(vertices);
        mIndexBuffers[Trim] = initIndexBuffer(indices);
    }

    const ClipMap::ClipMapConfig &ClipMap::getConfig() {
        return mConfig;
    }
}