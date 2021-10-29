#include "CForge/Graphics/RenderDevice.h"
#include "CForge/Graphics/Shader/SShaderManager.h"
#include <glad/glad.h>

#include "MapActor.h"
#include "Tile.h"

namespace Terrain {
    MapActor::MapActor() : IRenderableActor("MapActor", ATYPE_STATIC), m_pShader(nullptr) {}

    MapActor::~MapActor() {
        clear();
    }

    void MapActor::init() {
        clear();

        Tile tile = Tile(16);
        tile.calculateIndices(Tile::Corner);

        GLfloat* vertices = tile.getVertices();
        uint vertexBufferSize = tile.getVertexBufferSize();

        GLuint* indices = tile.getIndices();
        uint indexBufferSize = tile.getIndexBufferSize();

        m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, vertices, vertexBufferSize);
        m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, indices, indexBufferSize);

        m_VertexArray.init();
        m_VertexArray.bind();
        setBufferData();
        m_VertexArray.unbind();

        setShader();
    }

    void MapActor::clear() {
        m_VertexBuffer.clear();
        m_ElementBuffer.clear();
        m_VertexArray.clear();
        m_pShader = nullptr;
    }

    void MapActor::release() {
        delete this;
    }

    void MapActor::render(RenderDevice* renderDevice) {
        m_VertexArray.bind();
        renderDevice->activeShader(m_pShader);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_ElementBuffer.size()), GL_UNSIGNED_INT, nullptr);
    }

    void MapActor::setBufferData() {
        const GLint VertexSize = 3;
        const void* PositionOffset = nullptr;

        m_VertexBuffer.bind();
        m_ElementBuffer.bind();
        // enable vertex and UVS
        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), VertexSize,
                              GL_FLOAT, GL_FALSE, VertexSize * sizeof(GLfloat), PositionOffset);
    }

    void MapActor::setShader() {
        SShaderManager* shaderManager = SShaderManager::instance();

        std::vector<ShaderCode*> vsSources;
        std::vector<ShaderCode*> fsSources;
        std::string errorLog;

        ShaderCode* vertexShader =
            shaderManager->createShaderCode("Shader/MapShader.vert", "330 core",
                                            0, "", "");
        ShaderCode* fragmentShader =
            shaderManager->createShaderCode("Shader/MapShader.frag", "330 core",
                                            0, "", "");

        vsSources.push_back(vertexShader);
        fsSources.push_back(fragmentShader);

        m_pShader = shaderManager->buildShader(&vsSources, &fsSources, &errorLog);

        shaderManager->release();
    }
}
