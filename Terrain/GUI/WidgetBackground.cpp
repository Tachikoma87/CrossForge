#include "WidgetBackground.h"
#include <glad/glad.h>
#include <CForge/Graphics/RenderDevice.h>

using namespace CForge;

void WidgetBackground::setPosition(float x, float y) 
{
    m_x = x;
    m_y = y;
};
void WidgetBackground::setSize(float width, float height) 
{
    m_width = width;
    m_height = height;
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


void WidgetBackgroundColored::init(BackgroundStyle style, CForge::GLShader *pShader) 
{
    clear();

    float left = 0, right = 0.5, top = 0, bottom = 0.5;
    
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

    m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, QuadVertices, sizeof(QuadVertices));

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
void WidgetBackgroundColored::setBufferData()
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
WidgetBackgroundColored::~WidgetBackgroundColored()
{
    clear();
}
WidgetBackgroundColored::WidgetBackgroundColored()
{
    
}
WidgetBackground::WidgetBackground(): IRenderableActor("ScreenQuad", ATYPE_SCREENQUAD) 
{
    
}
WidgetBackground::~WidgetBackground()
{
    
}



