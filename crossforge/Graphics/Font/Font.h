/*****************************************************************************\
*                                                                           *
* File(s): FontFace.h and FontFace.cpp                                      *
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

#include <unordered_map>
#include "../GLBuffer.h"
#include "../GLVertexArray.h"

namespace CForge {

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
    class CFORGE_API Font: public CForgeObject {
    public:
        /** \brief Defines information about a single font style.
         *
         * You can add more font styles in a way similar to FontStyle1 or
         * FontStyle2. Note however, that in order for them to be usable by
         * widgets, they need to be manually loaded in CForge::GUI::loadFonts
         * at the current time.
         *
         * \ingroup GUI
         */
        struct FontStyle {
            std::string FileName;                               ///< The font file to load
            int32_t PixelSize;                                      ///< height of text in pixels
            std::u32string CharacterSet;

            FontStyle(void) {
                FileName = "";
                PixelSize = 16;
                CharacterSet = U" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¡¢£¤¥¦§¨©ª«¬®¯°±²";
            }//constructor

            ~FontStyle(void) {
                
            }//Constructor

            bool operator==(const FontStyle& Other) {
                bool Rval = true;
                if (FileName.compare(Other.FileName) == 0) Rval = false;
                if (CharacterSet.compare(Other.CharacterSet) == 0) Rval = false;
                if (PixelSize != Other.PixelSize) Rval = false;
                return Rval;
            }//operator==
        };

        /**
         * \brief Necessary information to use the font's glyphs.
         *
         * This struct wraps a glyph's bitmap with all the information required
         * to actually make use of it.
         *
         * \ingroup GUI
         */
            /*struct Glyph {
                FT_UInt index;              ///< The glyph's index within the font face. Not the same as its unicode value.
                uint8_t* bitmap;            ///< The glyphs rendered, grayscale image.
                FT_Vector bitmapSize;       ///< The glyph's pixel sizes.
                FT_Vector bitmapOffset;     ///< Offset of the bitmap relative to the canonical bounding box/size of the glyph.
                FT_Vector charMapOffset;    ///< The glyph's offset within the font texture. \sa prepareCharMap
                int advanceWidth;           ///< Canonical width of this character.
            };*/

            struct Glyph {
                uint32_t index;              ///< The glyph's index within the font face. Not the same as its unicode value.
                uint8_t* bitmap;            ///< The glyphs rendered, grayscale image.
                Eigen::Vector2i bitmapSize;       ///< The glyph's pixel sizes.
                Eigen::Vector2i bitmapOffset;     ///< Offset of the bitmap relative to the canonical bounding box/size of the glyph.
                Eigen::Vector2i charMapOffset;    ///< The glyph's offset within the font texture. \sa prepareCharMap
                int advanceWidth;           ///< Canonical width of this character.
            };

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
        Font(void);
        Font(const FontStyle Style);
        ~Font(void);

        void init(const FontStyle Style);
        void clear(void);
 

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
        Glyph glyph(char32_t character)const;

        Glyph glyph(char32_t character);

       

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

        /**
         * \brief Determines the expected pixel width for the input string if it was rendered.
         *
         * \param[in] text      A 32-Bit Unicode string to process.
         * \param[in] maxWidth  Unused. If a maximum string width is of concern, use \ref computeStringWidthMultiline instead.
         * \return              The expected string width in pixels.
         */
        int32_t computeStringWidth(std::u32string text, int maxWidth = -1)const;

        int32_t computeStringWidth(std::string text, int maxWidth = -1)const;

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
        int32_t computeStringWidthMultiline(std::u32string text, std::vector<std::u32string>* lines, int maxWidth = -1, int maxLines = -1)const;

        int32_t computeStringWidthMultiline(std::string Text, std::vector<std::string>* Lines, int MaxWidth = -1, int MaxLines = -1)const;

        //for render loop
        /** \brief Used by TextLine objects to bind the font texture. */
        void bind();

        /** \brief Returns the FontStyle this was initialised with. */
        FontStyle style()const;

    private:
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


        //     FT_Library library;
        void *m_pFaceHandle;               ///< FreeType font face handle.
        std::unordered_map <char32_t, Glyph> m_Glyphs;  ///< Caches all the glyphs that have been accessed/loaded.
        uint32_t m_TextureID;     ///< Holds the texture ID of the created OpenGL font texture.
        int32_t m_MapWidth, m_MapHeight;    ///< Size of the created font texture
        FontStyle m_Style;          ///< The FontStyle this has been initialised with.
        bool m_GlyphMapFinalized;     ///< Whether the font texture has been created yet. \sa getGlyph

    };

}//name space

#endif 