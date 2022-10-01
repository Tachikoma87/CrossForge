/*****************************************************************************\
*                                                                           *
* File(s): Font.h and Font.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Simon Kretzschmar, Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_FONT_H__
#define __CFORGE_FONT_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <unordered_map>

#include "GUIDefaults.h"
#include "../Graphics/Shader/GLShader.h"
#include "../Graphics/GLVertexArray.h"
#include "../Graphics/GLBuffer.h"
#include "../Graphics/RenderDevice.h"


namespace CForge {

    typedef struct glyph {
        FT_UInt index;
        FT_Vector pos;
        uint8_t* bitmap;
        FT_Vector bitmapSize;
        FT_Vector bitmapOffset;
        FT_Vector charMapOffset;
        int advanceWidth;
    } glyph_t;

    class CFORGE_API FontFace {
    public:
        FontFace(FontStyle style, FT_Library library);
        ~FontFace();

        glyph_t getGlyph(char32_t character);

        void prepareCharMap();

        int renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao);
        //     void computeBBox(std::u32string text, FT_Vector* positions, FT_BBox* pbbox);
        int computeStringWidth(std::u32string text, int maxWidth = -1);
        int computeStringWidthMultiline(std::u32string text, std::vector<std::u32string>* lines, int maxWidth = -1, int maxLines = -1);

        //for render loop
        void bind();

        FontStyle getStyle();

    private:
        //     FT_Library library;
        FT_Face face;
        std::unordered_map <char32_t, glyph_t> glyphs;
        unsigned int textureID;
        int mapWidth, mapHeight;
        FontStyle m_style;
        bool glyphMapFinalized;
    };


    class CFORGE_API TextLine {
    public:
        TextLine();
        ~TextLine();
        void init(FontFace* pFontFace, CForge::GLShader* pShader);
        void init(std::u32string text, FontFace* pFontFace, CForge::GLShader* pShader);
        void changeFont(FontFace* newFont);
        void setText(std::u32string text);
        void setColor(float r, float g, float b);
        void setPosition(float x, float y);
        void setRenderSize(uint32_t w, uint32_t h);   //Size of the window/framebuffer it gets rendered to
        void render(CForge::RenderDevice* pRDev);
        float getTextSize();
        int getTextWidth();
        int computeStringWidth(std::u32string textString);  //deprecated, better access the font directly if possible
    private:
        FontFace* m_pFont;
        CForge::GLShader* m_pShader;
        CForge::GLBuffer m_VertexBuffer;
        CForge::GLVertexArray m_VertexArray;
        int m_numVertices;
        int m_width;
        Eigen::Matrix4f m_projection;
        float textSize;
        float textColor[3];
    };

}//name space
#endif