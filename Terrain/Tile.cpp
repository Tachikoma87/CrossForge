#include <CForge/Graphics/RenderDevice.h>
#include <CForge/Graphics/Shader/SShaderManager.h>

#include "Tile.h"

namespace Terrain {
    Tile::Tile(uint32_t sideLength, GLTexture2D* heightMap) : mSideLength(sideLength), mHeightMap(heightMap) {
        initTiles();
        initLine();
        initTrim();
        initCross();
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

    uint32_t Tile::sideLength() const {
        return mSideLength;
    }

    void Tile::calculateVertices(vector<GLfloat>& vertices, uint32_t width, uint32_t height, float offsetX, float offsetY, bool swapPos) {
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

    void Tile::calculateIndices(vector<GLuint>& indices, uint32_t width, uint32_t height, TileVariant variant, uint32_t offset) {
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

    void Tile::addTriangle(vector<GLuint>& indices, uint32_t a, uint32_t b, uint32_t c) {
        indices.push_back(a);
        indices.push_back(c);
        indices.push_back(b);
    }

    void Tile::initBuffers(vector<GLfloat>& vertices, vector<GLuint>& indices, TileVariant variant) {
        GLBuffer vertexBuffer;
        vertexBuffer.init(GLBuffer::BTYPE_VERTEX,
                          GLBuffer::BUSAGE_STATIC_DRAW,
                          vertices.data(),
                          vertices.size() * sizeof(GLfloat));

        mIndexBufferSizes[variant] = indices.size() * sizeof(GLuint);

        GLBuffer indexBuffer;
        indexBuffer.init(GLBuffer::BTYPE_INDEX,
                         GLBuffer::BUSAGE_STATIC_DRAW,
                         indices.data(),
                         mIndexBufferSizes[variant]);

        initVertexArray(vertexBuffer, indexBuffer, variant);
    }

    void Tile::initVertexArray(GLBuffer& vertexBuffer, GLBuffer& indexBuffer, TileVariant variant) {
        mVertexArrays[variant].init();
        mVertexArrays[variant].bind();
        vertexBuffer.bind();
        indexBuffer.bind();

        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION),
                              2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

        mVertexArrays[variant].unbind();
    }

    void Tile::initTiles() {
        vector<GLfloat> vertices;
        calculateVertices(vertices, mSideLength, mSideLength);

        for (int variant = Normal; variant <= Corner; variant++) {
            vector<GLuint> indices;
            calculateIndices(indices, mSideLength, mSideLength, static_cast<TileVariant>(variant));

            initBuffers(vertices, indices, static_cast<TileVariant>(variant));
        }
    }

    void Tile::initLine() {
        vector<GLfloat> vertices;
        vector<GLuint> indices;
        calculateVertices(vertices, mSideLength, 2);
        calculateIndices(indices, mSideLength, 2, static_cast<TileVariant>(Edge));

        initBuffers(vertices, indices, Line);
    }

    void Tile::initTrim() {
        uint32_t sideLength = mSideLength * 2 + 1;
        uint32_t cornerIndex = (sideLength + 1) * 2;
        float positionOffset = static_cast<float>(mSideLength) + 1.0f;

        vector<GLfloat> vertices;
        vector<GLuint> indices;

        calculateVertices(vertices, 1, sideLength, -positionOffset);
        vertices.push_back(-positionOffset - 0.5f);
        vertices.push_back(-positionOffset - 0.5f);
        calculateVertices(vertices, 1, sideLength, -positionOffset, 0, true);

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

        initBuffers(vertices, indices, Trim);
    }

    void Tile::initCross() {
        float positionOffset = mSideLength / 2 + 1;
        uint32_t vertexOffset = 3 * (mSideLength + 1);

        vector<GLfloat> vertices;
        vector<GLuint> indices;

        calculateVertices(vertices, 2, mSideLength, 0, positionOffset);
        calculateVertices(vertices, 2, mSideLength, 0, -positionOffset);
        calculateVertices(vertices, mSideLength, 2, positionOffset);
        calculateVertices(vertices, mSideLength, 2, -positionOffset);
        calculateVertices(vertices, 2, 2);

        calculateIndices(indices, 2,  mSideLength, static_cast<TileVariant>(Normal));
        calculateIndices(indices, 2, mSideLength, static_cast<TileVariant>(Normal), vertexOffset);
        calculateIndices(indices, mSideLength, 2, static_cast<TileVariant>(Normal), 2 * vertexOffset);
        calculateIndices(indices, mSideLength, 2, static_cast<TileVariant>(Normal), 3 * vertexOffset);
        calculateIndices(indices, 2, 2, static_cast<TileVariant>(Normal), 4 * vertexOffset);

        initBuffers(vertices, indices, Cross);
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
}