#include <CForge/Graphics/RenderDevice.h>
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "Tile.h"

namespace Terrain {
    Tile::Tile(uint32_t sideLength, GLTexture2D* heightMap) : mSideLength(sideLength), mHeightMap(heightMap) {}

    Tile::~Tile() = default;

    void Tile::init() {
        initTileVertexArrays();
        initLineVertexArray();
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

    vector<GLfloat> Tile::calculateVertices(uint32_t width, uint32_t height) const {
        // width and height in triangle side count, vertex count one more
        vector<GLfloat> vertices;

        for (uint32_t y = 0; y <= height; y++) {
            for (uint32_t x = 0; x <= width; x++) {
                float percentX = (float) x / (float) width;
                float percentY = (float) y / (float) height;
                vertices.push_back((percentX - 0.5f) * (float) width);
                vertices.push_back((percentY - 0.5f) * (float) height);
            }
        }

        return vertices;
    }

    vector<GLuint> Tile::calculateIndices(uint32_t width, uint32_t height, TileVariant variant) const {
        vector<GLuint> indices;

        uint32_t vertexCount = width + 1;

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
        
        mVertexArrays[variant].init();
        mVertexArrays[variant].bind();
        vertexBuffer->bind();
        indexBuffer->bind();

        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION),
            VertexSize,
            GL_FLOAT,
            GL_FALSE,
            VertexSize * sizeof(GLfloat),
            0);

        mVertexArrays[variant].unbind();
    }

    void Tile::initTileVertexArrays() {
        vector<GLfloat> vertices = calculateVertices(mSideLength, mSideLength);

        for (int variant = Normal; variant <= Corner; variant++) {
            vector<GLuint> indices = calculateIndices(mSideLength, mSideLength, static_cast<TileVariant>(variant));

            initBuffers(&vertices, &indices, static_cast<TileVariant>(variant));
        }
    }

    void Tile::initLineVertexArray() {
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


}