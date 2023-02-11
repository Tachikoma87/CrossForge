#include <CForge/Graphics/OpenGLHeader.h>
#include <CForge/Graphics/Shader/SShaderManager.h>
#include <CForge/Utility/CForgeUtility.h>
#include "LineOfText.h"
#include <cuchar>
#include <CForge/Math/CForgeMath.h>

using namespace Eigen;

namespace CForge {

    LineOfText::LineOfText()
    {
        //use init instead
    }
    LineOfText::~LineOfText()
    {
        //     printf("cleaned up a TextLine.\n");
        //     m_VertexArray.~GLVertexArray();
        //     m_VertexBuffer.~GLBuffer();
    }

    void LineOfText::init(FontFace* pFontFace, CForge::GLShader* pShader)
    {
        m_pFont = pFontFace;
        m_TextSize = pFontFace->style().PixelSize;

        if(nullptr != pShader) m_pShader = pShader;
        else {
            // build default shader
            SShaderManager* pSMan = SShaderManager::instance();
            std::string ErrorLog;
            std::vector<ShaderCode*> VSSources;
            std::vector<ShaderCode*> FSSources;

#ifdef __EMSCRIPTEN__
            VSSources.push_back(pSMan->createShaderCode(pSMan->defaultShaderSources(SShaderManager::DEF_VS_TEXT)[0], "300 es", 0, "lowp"));
            FSSources.push_back(pSMan->createShaderCode(pSMan->defaultShaderSources(SShaderManager::DEF_FS_TEXT)[0], "300 es", 0, "lowp"));
#else
            VSSources.push_back(pSMan->createShaderCode(pSMan->defaultShaderSources(SShaderManager::DEF_VS_TEXT)[0], "330 core", 0, "mediump"));
            FSSources.push_back(pSMan->createShaderCode(pSMan->defaultShaderSources(SShaderManager::DEF_FS_TEXT)[0], "330 core", 0, "mediump"));
#endif
            m_pShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);
            if (!ErrorLog.empty()) throw CForgeExcept("Failed to build default text shader!");
            pSMan->release();
        }

        m_TextUBO.init();

        m_VertexArray.init();
        m_NumVertices = 0;
        canvasSize(1280, 720);
        position(0, 0);
        color(1, 1, 1);
    }

    void LineOfText::init(std::u32string Text, FontFace* pFontFace, CForge::GLShader* pShader)
    {
        init(pFontFace, pShader);
        text(Text);
    }

    void LineOfText::changeFont(FontFace* newFont)
    {
        m_pFont = newFont;
        m_TextSize = newFont->style().PixelSize;
    }

    void LineOfText::text(std::u32string Text)
    {
        //TODO: clean up old buffers if necessary?
        m_NumVertices = m_pFont->renderString(Text, &m_VertexBuffer, &m_VertexArray);
        m_Width = m_pFont->computeStringWidth(Text);  
    }

    void LineOfText::text(std::string Text) {
        std::u32string UString = CForgeUtility::convertTou32String(Text);
        text(UString);
        m_Text = Text;
    }//setText

    void LineOfText::position(float x, float y)
    {
        m_TextUBO.textPosition(Vector2f(x, y + m_TextSize));
        m_TextPosition = Vector2f(x, y);
    }
    void LineOfText::canvasSize(uint32_t Width, uint32_t Height)
    {
        m_TextUBO.canvasSize(Vector2f(Width, Height));
        m_CanvasSize = Vector2i(Width, Height);
    }//canvasSize

    void LineOfText::color(float r, float g, float b, float a)
    {
        color(Vector4f(r, g, b, a));
    }//color

    void LineOfText::color(const Eigen::Vector4f Color) {
        m_TextColor = Color;
        m_TextUBO.color(m_TextColor);
    }//color

    void LineOfText::render(CForge::RenderDevice* pRDev)
    {
        pRDev->activeShader(m_pShader);

        //Shader uniforms
        glActiveTexture(GL_TEXTURE0);
        m_pFont->bind();
        glUniform1i(m_pShader->uniformLocation("GlyphMap"), 0);

        uint32_t BindingPoint = m_pShader->uboBindingPoint(GLShader::DEFAULTUBO_TEXTDATA);
        if (BindingPoint != GL_INVALID_INDEX) m_TextUBO.bind(BindingPoint);

        m_VertexArray.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);
        m_VertexArray.unbind();
    }//render

    float LineOfText::textSize()
    {
        return m_TextSize;
    }//textSize

    int LineOfText::textWidth()
    {
        return m_Width;
    }//textWidth

    int LineOfText::computeStringWidth(std::u32string textString)
    {
        return m_pFont->computeStringWidth(textString);
    }//computeStringWidth

    int32_t LineOfText::computeStringWidth(const std::string Text) {
        return computeStringWidth(CForgeUtility::convertTou32String(Text));
    }//computeStringWidth

    std::string LineOfText::text(void)const {
        return m_Text;
    }//text

    Eigen::Vector4f LineOfText::color(void)const {
        return m_TextColor;
    }//color

    Eigen::Vector2f LineOfText::position(void)const {
        return m_TextPosition;
    }//position

    Eigen::Vector2i LineOfText::canvasSize(void)const {
        return m_CanvasSize;
    }//canvasSize

    const FontFace* LineOfText::fontFace(void)const {
        return m_pFont;
    }//fontFace

}//name space