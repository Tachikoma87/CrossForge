#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <string>
#include <unordered_map>

#include "GUIDefaults.h"

#include "CForge/Graphics/Shader/GLShader.h"
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>
#include <CForge/Graphics/RenderDevice.h>

typedef struct glyph {
    FT_UInt index;
    FT_Vector pos;
    uint8_t* bitmap;
    FT_Vector bitmapSize;
    FT_Vector bitmapOffset;
    FT_Vector charMapOffset;
    int advanceWidth;
} glyph_t;

class FontFace {
public:
    FontFace();
    ~FontFace();

    glyph_t getGlyph(char32_t character);

    void prepareCharMap();

    int renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao);
//     void computeBBox(std::u32string text, FT_Vector* positions, FT_BBox* pbbox);

    //for render loop
    void bind();

    FontStyle getStyle();

private:
    FT_Library library;
    FT_Face face;
    std::unordered_map <char32_t, glyph_t> glyphs;
    unsigned int textureID;
    int mapWidth, mapHeight;
    FontStyle style;
};


class TextLine {
public:
    TextLine();
    void init(FontFace* pFontFace, CForge::GLShader* pShader);
    void init(std::u32string text, FontFace* pFontFace, CForge::GLShader* pShader);
    void setText(std::u32string text);
    void setPosition(float x, float y);
    void render(CForge::RenderDevice* pRDev);
private:
    FontFace* m_pFont;
    CForge::GLShader* m_pShader;
    CForge::GLBuffer m_VertexBuffer;
    CForge::GLVertexArray m_VertexArray;
    int m_numVertices;
    Eigen::Matrix4f m_projection;
    float textSize;
};
