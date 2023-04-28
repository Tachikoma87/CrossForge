#include <cuchar>
#include "../../Math/CForgeMath.h"
#include "../../Utility/CForgeUtility.h"
#include "../Shader/SShaderManager.h"
#include "../OpenGLHeader.h"

#include "LineOfText.h"

using namespace Eigen;

namespace CForge {

    LineOfText::LineOfText(): CForgeObject("LineOfText")
    {
        m_pFont = nullptr;
        m_TextSize = 0;

        m_NumVertices = 0;
        m_Width = 0;
        m_TextColor = Vector4f::Ones();
        m_TextPosition = Vector2f(0.0f, 0.0f);
        m_CanvasSize = Vector2i(0, 0);
        m_Text = "";
        m_pShader = nullptr;

    }
    LineOfText::~LineOfText()
    {
        clear();
    }

    void LineOfText::init(Font* pFont, CForge::GLShader* pShader)
    {
        clear();
        m_pFont = pFont;
        m_TextSize = pFont->style().PixelSize;

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

    void LineOfText::init(Font* pFont, std::u32string Text, CForge::GLShader* pShader)
    {
        init(pFont, pShader);
        text(Text);
    }

    void LineOfText::init(Font* pFont, std::string Text, GLShader* pShader) {
        init(pFont, pShader);
        text(Text);
    }//initialize

    void LineOfText::clear(void) {
        m_pFont = nullptr;               
        m_TextSize = 0;                  

        m_NumVertices = 0;
        m_Width = 0;
        m_TextColor = Vector4f::Ones();
        m_TextPosition = Vector2f(0.0f, 0.0f);
        m_CanvasSize = Vector2i(0, 0);
        m_Text = "";

        m_TextUBO.clear();
        m_pShader = nullptr;
        m_VertexBuffer.clear();
        m_VertexArray.clear();
    }//clear

    void LineOfText::changeFont(Font* newFont)
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
        position(Vector2f(x, y));
    }

    void LineOfText::position(const Eigen::Vector2f Pos) {
        Vector2f P = Pos + Vector2f(0.0f, m_TextSize);
        P.x() = std::floor(P.x());
        P.y() = std::floor(P.y());

        m_TextUBO.textPosition(P);
        m_TextPosition = Pos;
    }//position

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

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_VertexArray.bind();
        glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);
        m_VertexArray.unbind();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
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

    const Font* LineOfText::font(void)const {
        return m_pFont;
    }//Font

}//name space