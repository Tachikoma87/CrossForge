#include <CForge/Graphics/RenderDevice.h>
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "Tile.h"

namespace Terrain {
    Tile::Tile(uint32_t sideLength, GLTexture2D* heightMap) : mSideLength(sideLength), mHeightMap(heightMap) {}

    Tile::~Tile() = default;

    void Tile::init() {
        initTiles();
        initLine();
        initTrim();
        initShader();
    }

    void Tile::render(RenderDevice* renderDevice, TileVariant variant) {
        mVertexArrays[variant].bind();
        renderDevice->activeShader(mShader);
        glActiveTexture(GL_TEXTURE0);
        mHeightMap->bind();
        glUniform1i(mShader->uniformLocation("HeightMap"), 0);
        glDrawElements(GL_TRIANGLES, mIndexBufferSizes[variant], GL_UNSIGNED_INT, nullptr);
    }

    vector<GLfloat> Tile::calculateVertices(uint32_t width, uint32_t height, float offsetX, bool swapPos) const {
        // width and height in triangle side count, vertex count one more
        vector<GLfloat> vertices;

        for (uint32_t y = 0; y <= height; y++) {
            for (uint32_t x = 0; x <= width; x++) {
                float percentX = static_cast<float>(x) / static_cast<float>(width);
                float percentY = static_cast<float>(y) / static_cast<float>(height);

                if (swapPos) {
                    vertices.push_back((percentY - 0.5f) * static_cast<float>(height));
                    vertices.push_back((percentX - 0.5f) * static_cast<float>(width) + offsetX);
                }
                else {
                    vertices.push_back((percentX - 0.5f) * static_cast<float>(width) + offsetX);
                    vertices.push_back((percentY - 0.5f) * static_cast<float>(height));
                }
            }
        }

        return vertices;
    }

    vector<GLuint> Tile::calculateIndices(uint32_t width, uint32_t height, TileVariant variant) const {
        vector<GLuint> indices;

        uint32_t vertexCount = width + 1;

        // a---b---c
        // | \ | / |
        // d---e---f
        // | / | \ |
        // g---h---i
        for (uint32_t y = 0; y < height; y += 2) {
            for (uint32_t x = 0; x < width; x += 2) {
                auto a = y * vertexCount + x;
                auto b = a + 1;
                auto c = a + 2;
                auto d = a + vertexCount;
                auto e = a + vertexCount + 1;
                auto f = a + vertexCount + 2;
                auto g = a + 2 * vertexCount;
                auto h = a + 2 * vertexCount + 1;
                auto i = a + 2 * vertexCount + 2;

                if (x == 0 && variant != Normal) {
                    addTriangle(&indices, e, g, a);
                } else {
                    addTriangle(&indices, e, g, d);
                    addTriangle(&indices, e, d, a);
                }

                if (y == 0 && variant == Corner) {
                    addTriangle(&indices, e, a, c);
                } else {
                    addTriangle(&indices, e, a, b);
                    addTriangle(&indices, e, b, c);
                }

                addTriangle(&indices, e, c, f);
                addTriangle(&indices, e, f, i);
                addTriangle(&indices, e, i, h);
                addTriangle(&indices, e, h, g);
            }
        }

        return indices;
    }

    void Tile::addTriangle(vector<GLuint>* indices, uint32_t a, uint32_t b, uint32_t c) {
        indices->push_back(a);
        indices->push_back(c);
        indices->push_back(b);
    }

    void Tile::initVertexArray(GLBuffer* vertexBuffer, GLBuffer* indexBuffer, TileVariant variant) {
        const GLint VertexSize = 2;
        const void* PositionOffset = nullptr;

        mVertexArrays[variant].init();
        mVertexArrays[variant].bind();
        vertexBuffer->bind();
        indexBuffer->bind();

        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION),
                              VertexSize, GL_FLOAT, GL_FALSE, VertexSize * sizeof(GLfloat), PositionOffset);

        mVertexArrays[variant].unbind();
    }

    void Tile::initTiles() {
        vector<GLfloat> vertices = calculateVertices(mSideLength, mSideLength);

        for (int variant = Normal; variant <= Corner; variant++) {
            vector<GLuint> indices = calculateIndices(mSideLength, mSideLength, static_cast<TileVariant>(variant));

            initBuffers(&vertices, &indices, static_cast<TileVariant>(variant));
        }
    }

    void Tile::initLine() {
        vector<GLfloat> vertices = calculateVertices(mSideLength, 2);
        vector<GLuint> indices = calculateIndices(mSideLength, 2, static_cast<TileVariant>(Edge));

        initBuffers(&vertices, &indices, Line);
    }

    void Tile::initShader() {
        SShaderManager* shaderManager = SShaderManager::instance();

        vector<ShaderCode*> vsSources;
        vector<ShaderCode*> fsSources;
        string errorLog;

        ShaderCode* vertexShader =
            shaderManager->createShaderCode("Shader/MapShader.vert", "330 core",
                                            0, "", "");
        ShaderCode* fragmentShader =
            shaderManager->createShaderCode("Shader/MapShader.frag", "330 core",
                                            0, "", "");

        vsSources.push_back(vertexShader);
        fsSources.push_back(fragmentShader);

        mShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

        shaderManager->release();
    }

    uint32_t Tile::getSideLength() const {
        return mSideLength;
    }

    void Tile::initBuffers(vector<GLfloat> *vertices, vector<GLuint> *indices, TileVariant variant) {
        GLBuffer vertexBuffer;
        vertexBuffer.init(GLBuffer::BTYPE_VERTEX,
                          GLBuffer::BUSAGE_STATIC_DRAW,
                          vertices->data(),
                          vertices->size() * sizeof(GLfloat));

        mIndexBufferSizes[variant] = static_cast<GLsizei>(indices->size() * sizeof(GLuint));

        GLBuffer indexBuffer;
        indexBuffer.init(GLBuffer::BTYPE_INDEX,
                         GLBuffer::BUSAGE_STATIC_DRAW,
                         indices->data(),
                         mIndexBufferSizes[variant]);

        initVertexArray(&vertexBuffer, &indexBuffer, variant);
    }

    void Tile::initTrim() {
        uint32_t sideLength = mSideLength * 2 + 1;
        uint32_t cornerIndex = (sideLength + 1) * 2;
        float offset = static_cast<float>(mSideLength) + 1.0f;

        vector<GLfloat> vertices = calculateVertices(1, sideLength, -offset);
        vector<GLfloat> otherVertices = calculateVertices(1, sideLength, -offset, true);

        // add corner vertex
        vertices.push_back(-offset - 0.5f);
        vertices.push_back(-offset - 0.5f);
        vertices.insert(vertices.end(), otherVertices.begin(), otherVertices.end());

        vector<GLuint> indices;
        // add corner quad
        addTriangle(&indices, cornerIndex, 1, 0);
        addTriangle(&indices, cornerIndex, cornerIndex + 1, cornerIndex + 2);

        for (uint32_t y = 0; y < sideLength; y++) {
            auto a = y * 2;
            auto b = a + 1;
            auto c = a + 2;
            auto d = a + 3;

            if (y % 2 == 0) {
                addTriangle(&indices, a, b, c);
                addTriangle(&indices, b, d, c);
            } else {
                addTriangle(&indices, a, b, d);
                addTriangle(&indices, a, d, c);
            }
        }

        for (uint32_t y = sideLength + 1; y < sideLength * 2 + 1; y++) {
            auto a = y * 2 + 1;
            auto b = a + 1;
            auto c = a + 2;
            auto d = a + 3;

            if (y % 2 == 0) {
                addTriangle(&indices, a, c, b);
                addTriangle(&indices, b, c, d);
            } else {
                addTriangle(&indices, a, d, b);
                addTriangle(&indices, a, c, d);
            }
        }

        initBuffers(&vertices, &indices, Trim);
    }
}