#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "../../AssetIO/File.h"
#include "../OpenGLHeader.h"
#include "../../Core/CrossForgeException.h"
#include "Font.h"
#include "SFontManager.h"
#include "../../Utility/CForgeUtility.h"

namespace CForge {

    Font::Font(void): CForgeObject("Font")
    {
        m_TextureID = GL_INVALID_INDEX;
        m_pFaceHandle = nullptr;
        m_MapWidth = m_MapHeight = 0;
        m_GlyphMapFinalized = false;
    }//Constructor

    Font::Font(const FontStyle Style): CForgeObject("Font") {
        m_TextureID = GL_INVALID_INDEX;
        m_pFaceHandle = nullptr;
        m_MapWidth = m_MapHeight = 0;
        m_GlyphMapFinalized = false;
        init(Style);
    }//Constructor

    Font::~Font() {
        clear();
    }

    void Font::init(FontStyle Style) {
        clear();

        m_Style = Style;
        FT_Face pFace = nullptr;
        SFontManager* pFMan = SFontManager::instance();

    
        if (!File::exists(m_Style.FileName)) throw CForgeExcept("File not found: " + m_Style.FileName);
        int error = FT_New_Face(static_cast<FT_Library>(pFMan->freetypeLibraryHandle()), m_Style.FileName.c_str(), 0, &pFace);

        if (error == FT_Err_Unknown_File_Format) {
            //File opened but not a supported font format
            throw CForgeExcept("Error reading Font " + m_Style.FileName + ": unsupported format ");
        }
        else if (error) {
            //File not found or unable to open or whatever
            throw CForgeExcept("Error opening Font " + m_Style.FileName);
        }
        m_pFaceHandle = static_cast<void*>(pFace);


        FT_Set_Pixel_Sizes(pFace, 0, m_Style.PixelSize);
        prepareCharMap();
    }//initialize

    void Font::clear(void) {
        for (auto x : m_Glyphs) {
            delete x.second.bitmap;
        }
        m_Glyphs.clear();
        if (nullptr != m_pFaceHandle) FT_Done_Face(static_cast<FT_Face>(m_pFaceHandle));
        if (glIsTexture(m_TextureID)) {
            glDeleteTextures(1, &m_TextureID);
            m_TextureID = GL_INVALID_INDEX;
        }
    }//clear


    int Font::renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao)
    {

        FT_Face FaceHandle = static_cast<FT_Face>(m_pFaceHandle);
        if (nullptr == FaceHandle) throw CForgeExcept("Font class was no properly initialized!");

        //Prepare the buffer data
        //Since GL_QUADS is not officially allowed for glDrawArrays
        //in the OpenGL 4 specification, triangles will be used instead,
        //resulting in 6 vertices per character with 4 components each,
        //2 position and 2 tex coords.
        const int numVertices = text.length() * 6;
        GLfloat* vertices = new GLfloat[numVertices * 4];

        //calculate the "descender" font value as pixels. This will be used
        //to correctly offset the baseline within the defined font pixel size.
        const float descender = m_Style.PixelSize / (FaceHandle->ascender / FaceHandle->descender - 1);

        //compute the glyphs' positions
        //the text string will start at (0, 0) at the bottom left on the
        //font face's baseline, positive numbers reaching to the top right.
        //It may go into negatives on characters like "p". The onscreen
        //positioning should then be handled by passing a corresponding
        //transformation matrix to the shader.
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(static_cast<FT_Face>(m_pFaceHandle));
        int previous_glyph_index = 0;
        for (int i = 0; i < text.length(); i++) {
            Glyph G = glyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && G.index) {
                FT_Vector delta;
                FT_Get_Kerning(FaceHandle, previous_glyph_index, G.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            int vertexIndex = i * 6 * 4;

            //position data
            float xpos = pen_x + G.bitmapOffset.x();
            float ypos = G.bitmapOffset.y() - G.bitmapSize.y() - descender;
            float w = G.bitmapSize.x(), h = G.bitmapSize.y(); //for easier access
            vertices[vertexIndex + 0] = xpos;
            vertices[vertexIndex + 1] = ypos + h;
            vertices[vertexIndex + 4] = xpos;
            vertices[vertexIndex + 5] = ypos;
            vertices[vertexIndex + 8] = xpos + w;
            vertices[vertexIndex + 9] = ypos;
            vertices[vertexIndex + 12] = xpos;
            vertices[vertexIndex + 13] = ypos + h;
            vertices[vertexIndex + 16] = xpos + w;
            vertices[vertexIndex + 17] = ypos;
            vertices[vertexIndex + 20] = xpos + w;
            vertices[vertexIndex + 21] = ypos + h;

            //texture coordinates on the glyph map
            float u = float(G.charMapOffset.x()) / m_MapWidth;
            float v = float(G.charMapOffset.y()) / m_MapHeight;
            float u_w = float(G.charMapOffset.x() + G.bitmapSize.x()) / m_MapWidth;
            float v_h = float(G.charMapOffset.y() + G.bitmapSize.y()) / m_MapHeight;
            vertices[vertexIndex + 2] = u;
            vertices[vertexIndex + 3] = v;
            vertices[vertexIndex + 6] = u;
            vertices[vertexIndex + 7] = v_h;
            vertices[vertexIndex + 10] = u_w;
            vertices[vertexIndex + 11] = v_h;
            vertices[vertexIndex + 14] = u;
            vertices[vertexIndex + 15] = v;
            vertices[vertexIndex + 18] = u_w;
            vertices[vertexIndex + 19] = v_h;
            vertices[vertexIndex + 22] = u_w;
            vertices[vertexIndex + 23] = v;

            pen_x += G.advanceWidth >> 6;
        }

        //Write to opengl buffer

        vao->bind();
        vbo->init(CForge::GLBuffer::BTYPE_VERTEX, CForge::GLBuffer::BUSAGE_STATIC_DRAW, vertices, sizeof(float) * numVertices * 4);
        vbo->bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

        vbo->unbind();
        vao->unbind();

        delete[] vertices;

        return numVertices;
    }

    int32_t Font::computeStringWidth(std::u32string text, int maxWidth) const
    {
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(static_cast<FT_Face>(m_pFaceHandle));
        int previous_glyph_index = 0;
        for (int i = 0; i < text.length(); i++) {
            Glyph G = glyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && G.index) {
                FT_Vector delta;
                FT_Get_Kerning(static_cast<FT_Face>(m_pFaceHandle), previous_glyph_index, G.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            pen_x += G.advanceWidth >> 6;
        }
        return pen_x;
    }//computeStringWidth

    int32_t Font::computeStringWidth(const std::string Text, int MaxWidth) const{
        return computeStringWidth(CForgeUtility::convertTou32String(Text), MaxWidth);
    }//computeStringWidth

    int32_t Font::computeStringWidthMultiline(std::u32string text, std::vector<std::u32string>* lines, int maxWidth, int maxLines)const
    {
        int ellipsis = 0;
        if (maxWidth > 0) {
            ellipsis = 3 * glyph(U'.').advanceWidth;
        }
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(static_cast<FT_Face>(m_pFaceHandle));
        int previous_glyph_index = 0;
        std::u32string currentLine;
        int maxLineWidth = 0;
        int numLines = 1;
        for (int i = 0; i < text.length(); i++) {
            if (text[i] == U'\n') {
                //start a new line
                lines->push_back(currentLine);
                currentLine.clear();
                maxLineWidth = std::max(maxLineWidth, pen_x);
                pen_x = 0;
                numLines++;
                continue;
            }
            Glyph G = glyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && G.index) {
                FT_Vector delta;
                FT_Get_Kerning(static_cast<FT_Face>(m_pFaceHandle), previous_glyph_index, G.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            if (maxWidth > 0 && pen_x + (G.advanceWidth >> 6) > maxWidth) {
                //new line or cut off
                if (maxLines > 0 && numLines >= maxLines) {
                    //cut off
                    while (pen_x + ellipsis > maxWidth && !currentLine.empty()) {
                        pen_x -= glyph(currentLine.back()).advanceWidth >> 6;
                        currentLine.pop_back();
                    }
                    pen_x += ellipsis;
                    currentLine += U"...";
                    break;
                }
                else {
                    // new line
                    lines->push_back(currentLine);
                    currentLine.clear();
                    maxLineWidth = std::max(maxLineWidth, pen_x);
                    pen_x = 0;
                    numLines++;
                }
            }
            pen_x += G.advanceWidth >> 6;
            currentLine.push_back(text[i]);
        }

        //don't forget the last line that was started
        lines->push_back(currentLine);
        maxLineWidth = std::max(maxLineWidth, pen_x);

        return maxLineWidth;
    }//computeStringWidthMultiline

    int32_t Font::computeStringWidthMultiline(std::string Text, std::vector<std::string>* Lines, int MaxWidth, int MaxLines) const{
        std::vector<std::u32string> U32Lines;
        int32_t Rval = computeStringWidthMultiline(CForgeUtility::convertTou32String(Text), &U32Lines, MaxWidth, MaxLines);

        if (nullptr != Lines) {
            for (auto i : U32Lines) Lines->push_back(CForgeUtility::convertToString(i));
        }
        return Rval;
    }//computeStringWidthMultiline

    void Font::prepareCharMap()
    {
        //CharactersToLoadIntoTexture is defined in GUIDefaults.h
        std::u32string charactersToLoad = m_Style.CharacterSet;
        charactersToLoad.push_back(U'\0'); //add the null character as fallback glyph

        //get glyph should not load new glyphs (during eg. text input) after were done
        //setting up the glyphmap. They wouldn't display correctly anyway.
        //This is realized by this flag.
        m_GlyphMapFinalized = false;

        //determine the size of the map necessary to fit all glyphs
        //
        //1) load all glyphs and determine the total width and height of the tallest glyph.
        //   Then use 2*sqrt(width*height) as a crude approximation to get a
        //   roughly square aspect ratio.
        int totalWidth = 0;
        int tallestGlyph = 0;
        for (auto i : charactersToLoad) {
            Glyph character = glyph(i);
            totalWidth += character.bitmapSize.x();
            if (tallestGlyph < character.bitmapSize.y()) tallestGlyph = character.bitmapSize.y();
        }
        const int targetWidth = int(sqrt(tallestGlyph * totalWidth));

        //2) pack the map theoretically first to get more accurate
        //   width and height numbers before writing the buffer
        //    and fill in the offset in the glyph_t struct
        //   Note that a small safety padding (+1) is needed for the sampling
        //   to not have glyphs bleed into each other on the vertex edges
        //TODO: maybe that's just an off-by-one positioning error though?
        //  (GL_NEAREST sampling helps a bit, but doesn't completely fix it)
        //   SOLVED: Projection matrix had issues, now the margin isn't needed.
        int maxRowWidth = 0;
        int totalHeight = 0;
        int currentRowWidth = 0, currentRowHeight = 0;
        for (auto i : charactersToLoad) {
            Glyph character = glyph(i);
            int newCurrentRowWidth = currentRowWidth + character.bitmapSize.x();
            if (newCurrentRowWidth < targetWidth) {
                //glyph fits into the row
                m_Glyphs[i].charMapOffset.x() = currentRowWidth;
                currentRowWidth = newCurrentRowWidth;
                if (character.bitmapSize.y() > currentRowHeight)
                    currentRowHeight = character.bitmapSize.y();
            }
            else {
                //glyph does not fit into the current row
                totalHeight += currentRowHeight;
                if (currentRowWidth > maxRowWidth)
                    maxRowWidth = currentRowWidth;
                currentRowWidth = character.bitmapSize.x();
                currentRowHeight = character.bitmapSize.y();
                m_Glyphs[i].charMapOffset.x() = 0;
            }
            m_Glyphs[i].charMapOffset.y() = totalHeight;
        }
        //don't forget the last row that was written to
        totalHeight += currentRowHeight;

        //Now write the map
        //
        //1) create the buffer
        const int numPixels = totalHeight * maxRowWidth;
        uint8_t* mapBuffer = new uint8_t[numPixels];

        //2) make sure it's filled with 0 (transparent)
        for (int i = 0; i < numPixels; i++) {
            mapBuffer[i] = 0;
        }

        //3) write the bitmap data
        currentRowWidth = 0;
        currentRowHeight = 0;
        for (auto i : charactersToLoad) {
            Glyph character = glyph(i);
            for (int y = 0; y < character.bitmapSize.y(); y++) {
                for (int x = 0; x < character.bitmapSize.x(); x++) {
                    const int j = (character.charMapOffset.y() + y) * maxRowWidth + (x + character.charMapOffset.x());
                    mapBuffer[j] = character.bitmap[y * character.bitmapSize.x() + x];
                }
            }
        }

        m_GlyphMapFinalized = true;

        m_MapWidth = maxRowWidth;
        m_MapHeight = totalHeight;

        //Now that we have the character map, store them in an OpenGL texture
        //1) required for widths that are not a multiple of 4
        //   (although the map could be made to align to that)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //2) create the texture using OpenGL directly instead of CForge.
        //   CForge's GLTexture makes some assumptions in its init() that
        //   don't apply here
        glGenTextures(1, &m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_TextureID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            maxRowWidth,
            totalHeight,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            mapBuffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);

        delete[] mapBuffer;
    }

    Font::Glyph Font::glyph(char32_t character) const{
        if (m_GlyphMapFinalized) {
            if (m_Glyphs.count(character) == 0) {
                return m_Glyphs.at(U'\0');
            }
            else {
                return m_Glyphs.at(character);
            }
        }
        else {
            return const_cast<Font*>(this)->glyph(character);
        }
    }//glyph

    Font::Glyph Font::glyph(char32_t character)
    {
        FT_Face FaceHandle = static_cast<FT_Face>(m_pFaceHandle);

        if (m_GlyphMapFinalized) {
            if (m_Glyphs.count(character) == 0) {
                return m_Glyphs.at(U'\0');
            }
            else {
                return m_Glyphs.at(character);
            }
        }
        else {
            if (m_Glyphs.count(character) == 0) {
                //fetch not-yet-loaded glyph using freetype
                m_Glyphs[character].index = FT_Get_Char_Index(FaceHandle, character);
                FT_Load_Glyph(FaceHandle, m_Glyphs[character].index, FT_LOAD_RENDER);
                m_Glyphs[character].advanceWidth = FaceHandle->glyph->advance.x;
                //copying the bitmap buffer because I think it'll be freed otherwise
                //and there's not much point to rendering it multiple times
                m_Glyphs[character].bitmapSize.x() = FaceHandle->glyph->bitmap.width;
                m_Glyphs[character].bitmapSize.y() = FaceHandle->glyph->bitmap.rows;
                m_Glyphs[character].bitmapOffset.x() = FaceHandle->glyph->bitmap_left;
                m_Glyphs[character].bitmapOffset.y() = FaceHandle->glyph->bitmap_top;
                //assumes ft_pixel_mode_gray which should be default
                const int numPixels = m_Glyphs[character].bitmapSize.x() * m_Glyphs[character].bitmapSize.y();
                m_Glyphs[character].bitmap = new uint8_t[numPixels];
                for (int i = 0; i < numPixels; i++) {
                    m_Glyphs[character].bitmap[i] = FaceHandle->glyph->bitmap.buffer[i];
                }
            }
            return m_Glyphs[character];
        }
    }

    void Font::bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_TextureID);
    }
    Font::FontStyle Font::style()const
    {
        return m_Style;
    }

}//name-space