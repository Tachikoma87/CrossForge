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
* MIT License without any warranty or guaranty to work properly.            *
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

#include <CForge/Graphics/Shader/GLShader.h>
#include <CForge/Graphics/Shader/GLShader.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>
#include <CForge/Graphics/RenderDevice.h>

#include "GUIDefaults.h"


namespace CForge {

    /**
     * \brief Necessary information to use the font's glyphs.
     *
     * This struct wraps a glyph's bitmap with all the information required
     * to actually make use of it.
     *
     * \ingroup GUI
     */
    typedef struct glyph {
        FT_UInt index;              ///< The glyph's index within the font face. Not the same as its unicode value.
        uint8_t* bitmap;            ///< The glyphs rendered, grayscale image.
        FT_Vector bitmapSize;       ///< The glyph's pixel sizes.
        FT_Vector bitmapOffset;     ///< Offset of the bitmap relative to the canonical bounding box/size of the glyph.
        FT_Vector charMapOffset;    ///< The glyph's offset within the font texture. \sa prepareCharMap
        int advanceWidth;           ///< Canonical width of this character.
    } glyph_t;

    /**
     * \brief Wrapper for a single font.
     *
     * This class wraps one font (that is, only one style and one size),
     * loads it using FreeType, creates an OpenGL texture of all the required
     * characters and provides methods to create graphical text lines
     * from strings.
     *
     * \ingroup GUI
     */
    class FontFace {
    public:

        /**
         * \brief Initializes the font.
         *
         * Loads the specified font style using FreeType and initialises
         * the class. Also calls \ref prepareCharMap to prepare the OpenGL
         * texture.
         *
         * \param[in] style     The style object of the font to load (includes path of fot file and the requested size).
         * \param[in] library   The FreeType library handle to use. One global instance can load multiple font faces.
         * \sa GUIDefaults.h
         */
        FontFace(FontStyle style, FT_Library library);
        ~FontFace();

        /**
         * \brief Loads one glyph (character) from the font.
         *
         * If the character was already loaded once before, the existing
         * glyph_t object will be returned instead. Otherwise if the OpenGL
         * texture has not been created yet (see \ref prepareCharMap), the
         * requested character will be loaded. If the texture does however already
         * exist, but the requested character hasn't been loaded, a placeholder
         * glyph will be returned instead.
         *
         * Multiple variations of the same character and glyphs for combinations
         * of characters (ligatures) are ignored. Only the default glyph for a
         * given codepoint is loaded.
         *
         * \param[in] character The 32-Bit Unicode representation of the character.
         */
        glyph_t getGlyph(char32_t character);

        /**
         * \brief Creates the OpenGL texture containing all required glyphs.
         *
         * Loads all characters specified in CForge::CharactersToLoadIntoTexture
         * and packs them into a grayscale OpenGL texture that can be sampled
         * to show text on screen.
         *
         * \sa GUIDefaults.h
         */
        void prepareCharMap();

        /**
         * \brief Creates OpenGL vertex data to display a string of text on screen.
         *
         *
         *
         * \param[in] text  A 32-Bit Unicode string to process.
         * \param[out] vbo  The vertex buffer that should be written into.
         * \param[out] vao  The vertex array the buffer is connected to.
         * \return          The number of vertices that has been created.
         */
        int renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao);
        //     void computeBBox(std::u32string text, FT_Vector* positions, FT_BBox* pbbox);

        /**
         * \brief Determines the expected pixel width for the input string if it was rendered.
         *
         * \param[in] text      A 32-Bit Unicode string to process.
         * \param[in] maxWidth  Unused. If a maximum string width is of concern, use \ref computeStringWidthMultiline instead.
         * \return              The expected string width in pixels.
         */
        int computeStringWidth(std::u32string text, int maxWidth = -1);

        /**
         * \brief Determines the expected pixel width for the multi-line input string if it was rendered.
         *
         * \todo
         * I don't think **automatic breaking up of long lines of text** has been
         * thoroughly tested yet, and in any case it should be improved to try
         * and break lines on spaces instead of whereever. So it is probably
         * best not to rely on this specific functionality at this point.
         *
         *
         * The input string will be split on new line characters (``U'\n'``),
         * and the individual lines will be written into the lines parameter.
         *
         * If a maxWidth is set, a string longer than that will be split and the
         * remainder moved to a new line. **Currently, this doesn't consider
         * spaces and will break lines mid word,** so try not to rely on this
         * feature as it is now.
         *
         * If more lines were to be created than allowed by the maxLines parameter,
         * then the last line will be cut off and show an ellipsis to indicate
         * more text has been cut off.
         *
         * The accompanying height should be calculated by the caller. The font
         * size in the \ref CForge::FontStyle objects is specified in pixels.
         * This function here can't do that, as the spacing between the text
         * lines is not set here. Each line will be an individual CForge::TextLine
         * object.
         *
         * The returned width is that of the longest line created.
         *
         * \param[in] text      A 32-Bit Unicode string to process.
         * \param[out] lines    Pointer to a list where the individual text lines will be written into.
         * \param[in] maxWidth  The maximum width allowed for a single line before starting a new one.
         * \param[in] maxLines  The maximum number of lines.
         * \return              The expected string width in pixels.
         */
        int computeStringWidthMultiline(std::u32string text, std::vector<std::u32string>* lines, int maxWidth = -1, int maxLines = -1);

        //for render loop
        /** \brief Used by TextLine objects to bind the font texture. */
        void bind();

        /** \brief Returns the FontStyle this was initialised with. */
        FontStyle getStyle();

    private:
        //     FT_Library library;
        FT_Face face;               ///< FreeType font face handle.
        std::unordered_map <char32_t, glyph_t> glyphs;  ///< Caches all the glyphs that have been accessed/loaded.
        unsigned int textureID;     ///< Holds the texture ID of the created OpenGL font texture.
        int mapWidth, mapHeight;    ///< Size of the created font texture
        FontStyle m_style;          ///< The FontStyle this has been initialised with.
        bool glyphMapFinalized;     ///< Whether the font texture has been created yet. \sa getGlyph
    };

    /**
     * \brief A class representing a single line of text for graphical display.
     *
     * \ingroup GUI
     */
    class TextLine {
    public:
        /** \brief Use \ref init() to initialise the object. */
        TextLine();
        ~TextLine();

        /**
         * \brief Sets up the TextLine object to use the specified font.
         *
         * \param[in] pFontFace Pointer to the FontFace object that should be used.
         * \param[in] pShader   Pointer to the text shader.
         */
        void init(FontFace* pFontFace, CForge::GLShader* pShader);

        /**
         * \brief Like #init(FontFace* pFontFace, CForge::GLShader* pShader)
         *        but also immediately loads the passed string.
         *
         * \param[in] text      32-Bit Unicode string that should be loaded/displayed.
         * \param[in] pFontFace Pointer to the FontFace object that should be used.
         * \param[in] pShader   Pointer to the text shader.
         */
        void init(std::u32string text, FontFace* pFontFace, CForge::GLShader* pShader);

        /** \brief Replaces the used FontFace with a new different one.
         *  \param[in] newFont Pointer to the FontFace object that should be used. */
        void changeFont(FontFace* newFont);

        /** \brief Sets the text that should be displayed.
         *  \param[in] text 32-Bit Unicode string that should be loaded. */
        void setText(std::u32string text);

        /** \brief Changes the text color. */
        void setColor(float r, float g, float b);

        /** \brief Sets the text position in window space.
         *  \sa CForge::mouseEventInfo */
        void setPosition(float x, float y);

        /**
         * \brief Pass the size of the render window.
         *
         * Because OpenGL shaders operate in a coordiate space from -1 to 1
         * but the GUI uses window coordinates from 0 to the window's pixel
         * size, we need to convert the coordinates. For that to work, the
         * window dimensions need to be known.
         */
        void setRenderSize(uint32_t w, uint32_t h);   //Size of the window/framebuffer it gets rendered to

        /** \brief Draw the text line to the screen using OpenGL.
         *  \param[in] pRDev Pointer to the CForge::RenderDevice used for drawing the GUI/Scene. */
        void render(CForge::RenderDevice* pRDev);

        /** \brief Getter for the FontFace's text size in pixels. */
        float getTextSize();

        /** \brief Getter for the currently loaded strings display width in pixels. */
        int getTextWidth();

        /** \brief Proxy for FontFace::computeStringWidth. Should be avoided in favour of the former. */
        int computeStringWidth(std::u32string textString);  //deprecated, better access the font directly if possible
    private:
        FontFace* m_pFont;                      ///< Pointer to the currently used FontFace object.
        CForge::GLShader* m_pShader;            ///< Pointer for the text shader.
        CForge::GLBuffer m_VertexBuffer;        ///< The vertex buffer to hold the text's vertex data.
        CForge::GLVertexArray m_VertexArray;    ///< A vertex array wrapping the vertex buffer.
        int m_numVertices;                      ///< The number of vertices of the current string.
        int m_width;                            ///< The pixel width of the current string.
        Eigen::Matrix4f m_projection;           ///< The projection matrix used to correctly scale and position the text on screen.
        float textSize;                         ///< The fonts text size in pixels.
        float textColor[3];                     ///< The current text colour.
    };

}//name space
#endif
