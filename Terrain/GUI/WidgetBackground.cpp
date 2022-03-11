#include "WidgetBackground.h"
#include <glad/glad.h>
#include <CForge/Graphics/RenderDevice.h>

using namespace CForge;

void WidgetBackground::setPosition(float x, float y) 
{
    m_x = x;
    m_y = y;
    updatePosition(false);
};
void WidgetBackground::setSize(float width, float height) 
{
    m_width = width;
    m_height = height;
    updatePosition(false);
};
void WidgetBackground::render(CForge::RenderDevice* pRDev) {
    m_VertexArray.bind();
    if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}//render
void WidgetBackground::release()
{
    delete this;
}

//adapted from the cforge screenquad
//TODO: the shader should be expanded to support different colors and perhaps some other
//styling elements in the future. Depending on how resizable and movable the GUI
//should become, it might also make sense to pass the position data as an uniform
//or add an transformation matrix
void WidgetBackgroundColored::init(BackgroundStyle style, CForge::GLShader *pShader) 
{
    clear();

// // 
    updatePosition(true);

    m_VertexArray.init();
    m_VertexArray.bind();
    setBufferData();
    m_VertexArray.unbind();
    
    m_pShader = pShader;
};
void WidgetBackgroundColored::clear()
{
    m_VertexArray.clear();
    m_VertexBuffer.clear();
    m_ElementBuffer.clear();
    m_pShader = nullptr;
}
void WidgetBackground::setBufferData()
{
    const uint32_t VertexSize = 4 * sizeof(float);
    const uint32_t PositionOffset = 0;
    const uint32_t UVOffset = 2 * sizeof(float);

    m_VertexBuffer.bind();
    // enable vertex and UVS
    glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
    glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)PositionOffset);
        
    glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
    glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)UVOffset);
}
void WidgetBackground::updatePosition(bool initialise)
{
    float left = m_x, right = m_x + m_width, top = m_y, bottom = m_y + m_height;
    
//     mapping from [0,1] -> [-1,1] (NDC)
    left = left * 2.0f - 1.0f;
    top = top * 2.0f - 1.0f;
    right = right * 2.0f - 1.0f;
    bottom = bottom * 2.0f - 1.0f;

    top *= -1.0f;
    bottom *= -1.0f;

    float QuadVertices[] = {
        left, bottom,		0.0f, 0.0f,
        right, top,			1.0f, 1.0f,
        left, top,			0.0f, 1.0f,
        
        left, bottom,		0.0f, 0.0f,
        right, bottom,		1.0f, 0.0f,
        right, top,			1.0f, 1.0f
    };
    
    if (initialise) {
        m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, QuadVertices, sizeof(QuadVertices));
    } else {
        m_VertexBuffer.bufferData(QuadVertices, sizeof(QuadVertices));
    }
    
}
WidgetBackgroundColored::~WidgetBackgroundColored()
{
    clear();
}
WidgetBackgroundColored::WidgetBackgroundColored()
{
    
}
WidgetBackground::WidgetBackground(): IRenderableActor("ScreenQuad", ATYPE_SCREENQUAD) 
{
    m_x = 0;
    m_y = 0;
    m_width = 0.2;
    m_height = 0.2;
}
WidgetBackground::~WidgetBackground()
{
    
}



