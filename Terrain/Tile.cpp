#include <CForge/Graphics/RenderDevice.h>
#include "CForge/Graphics/Shader/SShaderManager.h"
#include "Tile.h"

namespace Terrain {
    Tile::Tile(uint sideLength, GLTexture2D* heightMap) : mSideLength(sideLength), mVertexCount(sideLength + 1), mHeightMap(heightMap) {}

    Tile::~Tile() = default;

    void Tile::init() {
        vector<GLfloat> vertices = calculateVertices();

        GLBuffer vertexBuffer;
        vertexBuffer.init(GLBuffer::BTYPE_VERTEX,
                           GLBuffer::BUSAGE_STATIC_DRAW,
                           vertices.data(),
                           vertices.size() * sizeof(GLfloat));

        for (int variant = Normal; variant <= Corner; variant++) {
            vector<GLuint> indices = calculateIndices(static_cast<TileVariant>(variant));

            mIndexBufferSizes[variant] = static_cast<GLsizei>(indices.size() * sizeof(GLuint));

            GLBuffer indexBuffer;
            indexBuffer.init(GLBuffer::BTYPE_INDEX,
                             GLBuffer::BUSAGE_STATIC_DRAW,
                             indices.data(),
                             mIndexBufferSizes[variant]);

            initVertexArray(&vertexBuffer, &indexBuffer, static_cast<TileVariant>(variant));
        }

        initShader();
    }

    void Tile::render(RenderDevice* renderDevice, TileVariant variant) {
        mVertexArrays[variant].bind();
        renderDevice->activeShader(mShader);
        mShader->bindTexture(GLShader::DEFAULTTEX_ALBEDO, mHeightMap);
        glDrawElements(GL_TRIANGLES, mIndexBufferSizes[variant], GL_UNSIGNED_INT, nullptr);
    }

    vector<GLfloat> Tile::calculateVertices() const {
        vector<GLfloat> vertices;

        for (int y = 0; y < mVertexCount; y++) {
            for (int x = 0; x < mVertexCount; x++) {
                float percentX = (float) x / (float) mSideLength;
                float percentY = (float) y / (float) mSideLength;
                vertices.push_back((percentX - 0.5f) * (float) mSideLength);
                vertices.push_back((percentY - 0.5f) * (float) mSideLength);
            }
        }

        return vertices;
    }

    vector<GLuint> Tile::calculateIndices(TileVariant variant) const {
        vector<GLuint> indices;

        for (int y = 0; y < mSideLength; y += 2) {
            for (int x = 0; x < mSideLength; x += 2) {
                auto a = y * mVertexCount + x;
                auto b = a + 1;
                auto c = a + 2;
                auto d = a + mVertexCount;
                auto e = a + mVertexCount + 1;
                auto f = a + mVertexCount + 2;
                auto g = a + 2 * mVertexCount;
                auto h = a + 2 * mVertexCount + 1;
                auto i = a + 2 * mVertexCount + 2;

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

    void Tile::addTriangle(vector<GLuint>* indices, uint a, uint b, uint c) {
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
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), VertexSize,
                              GL_FLOAT, GL_FALSE, VertexSize * sizeof(GLfloat), PositionOffset);

        mVertexArrays[variant].unbind();
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

    uint Tile::getSideLength() const {
        return mSideLength;
    }
}