
#include <CForge/Graphics/OpenGLHeader.h>
#include "WidgetBackground.h"
#include "Widget.h"
#include <CForge/Graphics/RenderDevice.h>

//adapted from the cforge screenquad
//TODO: the shader should be expanded to support different colors and perhaps some other
//styling elements in the future. Depending on how resizable and movable the GUI
//should become, it might also make sense to pass the position data as an uniform
//or add an transformation matrix

namespace CForge {

    WidgetBackground::WidgetBackground(BaseWidget* parent, GUI* root) : IRenderableActor("WidgetBackground", ATYPE_SCREENQUAD)
    {
        m_parent = parent;

        m_pShader = root->BackgroundColoredShader;

        m_projection = Eigen::Matrix4f::Identity();
        m_projection(0, 0) = 2.0f / root->getWindowWidth();
        m_projection(1, 1) = 2.0f / root->getWindowHeight();
        updatePosition();
    }
    void WidgetBackground::initBuffers()
    {
        updateSize(true);
        m_VertexArray.init();
        m_VertexArray.bind();
        setBufferData();
        m_VertexArray.unbind();
    }
    WidgetBackground::~WidgetBackground()
    {

    }
    void WidgetBackground::release()
    {
        delete this;
    }
    void WidgetBackground::setPosition(float x, float y)
    {
        //TODO currently does not work for rotated text
        // consider using proper matrix operations in the future
        m_projection(0, 3) = m_projection(0, 0) * x - 1;
        m_projection(1, 3) = 1 - m_projection(1, 1) * y;
    };
    void WidgetBackground::updatePosition()
    {
        auto pos = m_parent->getPosition();
        setPosition(pos.x(), pos.y());
    }
    void WidgetBackground::setColor(float r, float g, float b, float a)
    {
        m_color[0] = r;
        m_color[1] = g;
        m_color[2] = b;
        if (a > 0) m_color[3] = a;
    }
    void WidgetBackground::setColor(float color[3], float a)
    {
        m_color[0] = color[0];
        m_color[1] = color[1];
        m_color[2] = color[2];
        if (a > 0) m_color[3] = a;
    }

    void WidgetBackground::setBufferData()
    {
        const uint32_t VertexSize = 4 * sizeof(float);
        const uint32_t PositionOffset = 0;
        const uint32_t UVOffset = 2 * sizeof(float);

        m_VertexBuffer.bind();
        // enable vertex and UVS
        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_POSITION), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)(uint64_t(PositionOffset)));

        glEnableVertexAttribArray(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW));
        glVertexAttribPointer(GLShader::attribArrayIndex(GLShader::ATTRIB_UVW), 2, GL_FLOAT, GL_FALSE, VertexSize, (const void*)(uint64_t(UVOffset)));
    }
    void WidgetBackground::updateSize(bool initialise)
    {
        //only buffer some quad of the required width and height,
        //position will be handled by the projection matrix

        float left = 0;
        float right = m_parent->getWidth();
        float top = 0;
        float bottom = -m_parent->getHeight();

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
        }
        else {
            m_VertexBuffer.bufferSubData(0, sizeof(QuadVertices), QuadVertices);
        }

    }
    void WidgetBackground::setRenderSize(uint32_t w, uint32_t h)
    {
        //just like setPosition(), this assumes there's no rotation or other
        //transformations of that kind
        float x = m_projection(0, 3) / m_projection(0, 0);
        float y = m_projection(1, 3) / m_projection(1, 1);
        m_projection(0, 0) = 2.0f / w;
        m_projection(1, 1) = 2.0f / h;
        setPosition(x, y);
    }


    WidgetBackgroundColored::WidgetBackgroundColored(BaseWidget* parent, GUI* root) : WidgetBackground(parent, root)
    {
        initBuffers();
        BackgroundStyle defaults;
        setColor(defaults.Color, defaults.Alpha);
    }
    WidgetBackgroundColored::~WidgetBackgroundColored()
    {
        clear();
    }
    void WidgetBackgroundColored::clear()
    {
        m_VertexArray.clear();
        m_VertexBuffer.clear();
        m_ElementBuffer.clear();
        m_pShader = nullptr;
    }
    void WidgetBackgroundColored::render(CForge::RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
        m_VertexArray.bind();
        if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
        glUniform4fv(m_pShader->uniformLocation("color"), 1, m_color);
        glUniformMatrix4fv(m_pShader->uniformLocation("projection"), 1, GL_FALSE, m_projection.data());
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }//render



    WidgetBackgroundBorder::WidgetBackgroundBorder(BaseWidget* parent, GUI* root) : WidgetBackground(parent, root)
    {
        initBuffers();
        BorderStyle defaults;
        m_lineWidth = defaults.LineWidth;
        setColor(defaults.Color, defaults.Alpha);
    }
    WidgetBackgroundBorder::~WidgetBackgroundBorder()
    {
        clear();
    }
    void WidgetBackgroundBorder::clear()
    {
        m_VertexArray.clear();
        m_VertexBuffer.clear();
        m_ElementBuffer.clear();
        m_pShader = nullptr;
    }
    void WidgetBackgroundBorder::setLineWidth(float lw)
    {
        m_lineWidth = lw;
    }
    void WidgetBackgroundBorder::updateSize(bool initialise)
    {
        //only buffer some quad of the required width and height,
        //position will be handled by the projection matrix

        float left = 0;
        float right = m_parent->getWidth();
        float top = 0;
        float bottom = -m_parent->getHeight();

        float QuadVertices[] = {
            left, bottom,		0.0f, 0.0f,
            right, bottom,		1.0f, 0.0f,
            right, top,			1.0f, 1.0f,
            left, top,			0.0f, 1.0f,
        };

        if (initialise) {
            m_VertexBuffer.init(GLBuffer::BTYPE_VERTEX, GLBuffer::BUSAGE_STATIC_DRAW, QuadVertices, sizeof(QuadVertices));
        }
        else {
            m_VertexBuffer.bufferSubData(0, sizeof(QuadVertices), QuadVertices);
        }
    }

    void WidgetBackgroundBorder::render(class CForge::RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale)
    {
        m_VertexArray.bind();
        if (nullptr != m_pShader) pRDev->activeShader(m_pShader);
        glUniform4fv(m_pShader->uniformLocation("color"), 1, m_color);
        glUniformMatrix4fv(m_pShader->uniformLocation("projection"), 1, GL_FALSE, m_projection.data());
        glLineWidth(m_lineWidth);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }//render

}//name space