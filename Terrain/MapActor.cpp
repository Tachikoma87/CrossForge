#include <glad/glad.h>
#include "../CForge/Graphics/RenderDevice.h"
#include "MapActor.h"
#include "../CForge/Graphics/Shader/SShaderManager.h"

namespace Terrain {
    MapActor::MapActor() : IRenderableActor("MapActor", ATYPE_STATIC), m_pShader(nullptr) {}

    MapActor::~MapActor() {
        clear();
    }

    void MapActor::init() {
        clear();

        GLfloat vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };

        GLuint indices[] = {
            0, 1, 2, 0, 2, 3
        };

        m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, vertices, sizeof(vertices));
        m_ElementBuffer.init(GLBuffer::BTYPE_INDEX, GLBuffer::BUSAGE_STATIC_DRAW, indices, sizeof(indices));

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

    void MapActor::render(RenderDevice *renderDevice) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2);

        m_VertexArray.bind();
        renderDevice->activeShader(m_pShader);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_ElementBuffer.size()), GL_UNSIGNED_INT, nullptr);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
