#include <CForge/Graphics/OpenGLHeader.h>

#include <iostream>
#include <fstream>

#include <CForge/AssetIO/SAssetIO.h>
#include "Font.h"

namespace CForge {

    FontFace::FontFace(FontStyle style, FT_Library library)
    {
        m_style = style;
        int error = FT_New_Face(library, m_style.FileName.c_str(), 0, &face);
        if (error == FT_Err_Unknown_File_Format) {
            //File opened but not a supported font format
            printf("Error reading Font %s: unsupported format\n", m_style.FileName.c_str());
        }
        else if (error) {
            //File not found or unable to open or whatever
            printf("Error opening Font %s\n", m_style.FileName.c_str());
        }
        FT_Set_Pixel_Sizes(face, 0, m_style.PixelSize);

        prepareCharMap();

    }
    FontFace::~FontFace() {
        for (auto x : glyphs) {
            delete x.second.bitmap;
        }
        FT_Done_Face(face);
        if (glIsTexture(textureID)) {
            glDeleteTextures(1, &textureID);
            textureID = GL_INVALID_INDEX;
        }
    }

    int FontFace::renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao)
    {
        //Prepare the buffer data
        //Since GL_QUADS is not officially allowed for glDrawArrays
        //in the OpenGL 4 specification, triangles will be used instead,
        //resulting in 6 vertices per character with 4 components each,
        //2 position and 2 tex coords.
        const int numVertices = text.length() * 6;
        GLfloat* vertices = new GLfloat[numVertices * 4];

        //calculate the "descender" font value as pixels. This will be used
        //to correctly offset the baseline within the defined font pixel size.
        const float descender = m_style.PixelSize / (face->ascender / face->descender - 1);

        //compute the glyphs' positions
        //the text string will start at (0, 0) at the bottom left on the
        //font face's baseline, positive numbers reaching to the top right.
        //It may go into negatives on characters like "p". The onscreen
        //positioning should then be handled by passing a corresponding
        //transformation matrix to the shader.
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(face);
        int previous_glyph_index = 0;
        for (int i = 0; i < text.length(); i++) {
            glyph_t glyph = getGlyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && glyph.index) {
                FT_Vector delta;
                FT_Get_Kerning(face, previous_glyph_index, glyph.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            int vertexIndex = i * 6 * 4;

            //position data
            float xpos = pen_x + glyph.bitmapOffset.x;
            float ypos = glyph.bitmapOffset.y - glyph.bitmapSize.y - descender;
            float w = glyph.bitmapSize.x, h = glyph.bitmapSize.y; //for easier access
            vertices[vertexIndex] = xpos;
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
            float u = float(glyph.charMapOffset.x) / mapWidth;
            float v = float(glyph.charMapOffset.y) / mapHeight;
            float u_w = float(glyph.charMapOffset.x + glyph.bitmapSize.x) / mapWidth;
            float v_h = float(glyph.charMapOffset.y + glyph.bitmapSize.y) / mapHeight;
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

            pen_x += glyph.advanceWidth >> 6;
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

    // currently unused
    // void FontFace::computeBBox(std::u32string text, FT_Vector* positions, FT_BBox* pbbox)
    // {
    //     FT_BBox  bbox;
    //     FT_BBox  glyph_bbox;
    //     //initialise bbox
    //     bbox.xMin = bbox.yMin =  32000;
    //     bbox.xMax = bbox.yMax = -32000;
    //
    //     for (int i = 0; i < text.length(); i++) {
    //         FT_Glyph_Get_CBox( glyphs[text[i]].image, ft_glyph_bbox_pixels, &glyph_bbox );
    //
    //         glyph_bbox.xMin += positions[i].x;
    //         glyph_bbox.xMax += positions[i].x;
    //         glyph_bbox.yMin += positions[i].y;
    //         glyph_bbox.yMax += positions[i].y;
    //
    //         if ( glyph_bbox.xMin < bbox.xMin )
    //             bbox.xMin = glyph_bbox.xMin;
    //         if ( glyph_bbox.yMin < bbox.yMin )
    //             bbox.yMin = glyph_bbox.yMin;
    //         if ( glyph_bbox.xMax > bbox.xMax )
    //             bbox.xMax = glyph_bbox.xMax;
    //         if ( glyph_bbox.yMax > bbox.yMax )
    //             bbox.yMax = glyph_bbox.yMax;
    //     }
    //     if (bbox.xMin > bbox.xMax ) {
    //         bbox.xMin = 0;
    //         bbox.yMin = 0;
    //         bbox.xMax = 0;
    //         bbox.yMax = 0;
    //     }
    //     *pbbox = bbox;
    // }
    int FontFace::computeStringWidth(std::u32string text, int maxWidth)
    {
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(face);
        int previous_glyph_index = 0;
        for (int i = 0; i < text.length(); i++) {
            glyph_t glyph = getGlyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && glyph.index) {
                FT_Vector delta;
                FT_Get_Kerning(face, previous_glyph_index, glyph.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            pen_x += glyph.advanceWidth >> 6;
        }
        return pen_x;
    }
    int FontFace::computeStringWidthMultiline(std::u32string text, std::vector<std::u32string>* lines, int maxWidth, int maxLines)
    {
        int ellipsis = 0;
        if (maxWidth > 0) {
            ellipsis = 3 * getGlyph(U'.').advanceWidth;
        }
        int pen_x = 0;
        bool use_kerning = FT_HAS_KERNING(face);
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
            glyph_t glyph = getGlyph(text[i]);
            //consider kerning values for nicer output
            if (use_kerning && previous_glyph_index && glyph.index) {
                FT_Vector delta;
                FT_Get_Kerning(face, previous_glyph_index, glyph.index, FT_KERNING_DEFAULT, &delta);
                pen_x += delta.x >> 6; //spacing values are returned in 1/64th of a pixel
            }
            if (maxWidth > 0 && pen_x + (glyph.advanceWidth >> 6) > maxWidth) {
                //new line or cut off
                if (maxLines > 0 && numLines >= maxLines) {
                    //cut off
                    while (pen_x + ellipsis > maxWidth && !currentLine.empty()) {
                        pen_x -= getGlyph(currentLine.back()).advanceWidth >> 6;
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
            pen_x += glyph.advanceWidth >> 6;
            currentLine.push_back(text[i]);
        }

        //don't forget the last line that was started
        lines->push_back(currentLine);
        maxLineWidth = std::max(maxLineWidth, pen_x);

        return maxLineWidth;
    }


    void FontFace::prepareCharMap()
    {
        //CharactersToLoadIntoTexture is defined in GUIDefaults.h
        std::u32string charactersToLoad = CharactersToLoadIntoTexture;
        charactersToLoad.push_back(U'\0'); //add the null character as fallback glyph

        //get glyph should not load new glyphs (during eg. text input) after were done
        //setting up the glyphmap. They wouldn't display correctly anyway.
        //This is realized by this flag.
        glyphMapFinalized = false;

        //determine the size of the map necessary to fit all glyphs
        //
        //1) load all glyphs and determine the total width and height of the tallest glyph.
        //   Then use 2*sqrt(width*height) as a crude approximation to get a
        //   roughly square aspect ratio.
        int totalWidth = 0;
        int tallestGlyph = 0;
        for (auto i : charactersToLoad) {
            glyph_t character = getGlyph(i);
            totalWidth += character.bitmapSize.x;
            if (tallestGlyph < character.bitmapSize.y) tallestGlyph = character.bitmapSize.y;
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
            glyph_t character = getGlyph(i);
            int newCurrentRowWidth = currentRowWidth + character.bitmapSize.x;
            if (newCurrentRowWidth < targetWidth) {
                //glyph fits into the row
                glyphs[i].charMapOffset.x = currentRowWidth;
                currentRowWidth = newCurrentRowWidth;
                if (character.bitmapSize.y > currentRowHeight)
                    currentRowHeight = character.bitmapSize.y;
            }
            else {
                //glyph does not fit into the current row
                totalHeight += currentRowHeight;
                if (currentRowWidth > maxRowWidth)
                    maxRowWidth = currentRowWidth;
                currentRowWidth = character.bitmapSize.x;
                currentRowHeight = character.bitmapSize.y;
                glyphs[i].charMapOffset.x = 0;
            }
            glyphs[i].charMapOffset.y = totalHeight;
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
            glyph_t character = getGlyph(i);
            for (int y = 0; y < character.bitmapSize.y; y++) {
                for (int x = 0; x < character.bitmapSize.x; x++) {
                    const int j = (character.charMapOffset.y + y) * maxRowWidth + (x + character.charMapOffset.x);
                    mapBuffer[j] = character.bitmap[y * character.bitmapSize.x + x];
                }
            }
        }

        glyphMapFinalized = true;

        mapWidth = maxRowWidth;
        mapHeight = totalHeight;

        //Now that we have the character map, store them in an OpenGL texture
        //1) required for widths that are not a multiple of 4
        //   (although the map could be made to align to that)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        //2) create the texture using OpenGL directly instead of CForge.
        //   CForge's GLTexture makes some assumptions in its init() that
        //   don't apply here
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
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

        //output glyph map to check alignment
    //     CForge::T2DImage<uint8_t> glyphMapImage;
    //     CForge::GraphicsUtility::retrieveColorTexture(textureID, &glyphMapImage);
    //     CForge::AssetIO::store("Assets/font.png", &glyphMapImage);
        std::ofstream binFile("font.bin", std::ios::out | std::ios::binary);
        binFile.write((char*)mapBuffer, mapWidth * mapHeight);
        binFile.close();

        delete[] mapBuffer;
    }

    glyph_t FontFace::getGlyph(char32_t character)
    {
        if (glyphMapFinalized) {
            if (glyphs.count(character) == 0) {
                return glyphs[U'\0'];
            }
            else {
                return glyphs[character];
            }
        }
        else {
            if (glyphs.count(character) == 0) {
                //fetch not-yet-loaded glyph using freetype
                glyphs[character].index = FT_Get_Char_Index(face, character);
                FT_Load_Glyph(face, glyphs[character].index, FT_LOAD_RENDER);
                glyphs[character].advanceWidth = face->glyph->advance.x;
                //copying the bitmap buffer because I think it'll be freed otherwise
                //and there's not much point to rendering it multiple times
                glyphs[character].bitmapSize.x = face->glyph->bitmap.width;
                glyphs[character].bitmapSize.y = face->glyph->bitmap.rows;
                glyphs[character].bitmapOffset.x = face->glyph->bitmap_left;
                glyphs[character].bitmapOffset.y = face->glyph->bitmap_top;
                //assumes ft_pixel_mode_gray which should be default
                const int numPixels = glyphs[character].bitmapSize.x * glyphs[character].bitmapSize.y;
                glyphs[character].bitmap = new uint8_t[numPixels];
                for (int i = 0; i < numPixels; i++) {
                    glyphs[character].bitmap[i] = face->glyph->bitmap.buffer[i];
                }
            }
            return glyphs[character];
        }
    }

    void FontFace::bind()
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    FontStyle FontFace::getStyle()
    {
        return m_style;
    }



    TextLine::TextLine()
    {
        //use init instead
    }
    TextLine::~TextLine()
    {
        //     printf("cleaned up a TextLine.\n");
        //     m_VertexArray.~GLVertexArray();
        //     m_VertexBuffer.~GLBuffer();
    }

    void TextLine::init(FontFace* pFontFace, CForge::GLShader* pShader)
    {
        m_pFont = pFontFace;
        textSize = pFontFace->getStyle().PixelSize;
        m_pShader = pShader;
        m_VertexArray.init();
        m_numVertices = 0;
        m_projection = Eigen::Matrix4f::Identity();
        //some default scale, don't forget to set the proper size with setRenderSize()!
        float scale_x, scale_y;
        scale_x = scale_y = 2.0f / 720.0f;
        m_projection(0, 0) = scale_x;
        m_projection(1, 1) = scale_y;
        setColor(1, 1, 1);
    }

    void TextLine::init(std::u32string text, FontFace* pFontFace, CForge::GLShader* pShader)
    {
        init(pFontFace, pShader);
        setText(text);
    }

    void TextLine::changeFont(FontFace* newFont)
    {
        m_pFont = newFont;
        textSize = newFont->getStyle().PixelSize;
    }

    void TextLine::setText(std::u32string text)
    {
        //TODO: clean up old buffers if necessary?
        m_numVertices = m_pFont->renderString(text, &m_VertexBuffer, &m_VertexArray);
        m_width = m_pFont->computeStringWidth(text);
    }

    void TextLine::setPosition(float x, float y)
    {
        //TODO currently does not work for rotated text
        // consider using proper matrix operations in the future

        //To make it abide to the same top-left based coordinate system
        //that mouse clicks use, we need to move the text down by its size,
        //which is done by adding it to the y coordinate.
        m_projection(0, 3) = m_projection(0, 0) * x - 1;
        m_projection(1, 3) = 1 - m_projection(1, 1) * (y + textSize);
    }
    void TextLine::setRenderSize(uint32_t w, uint32_t h)
    {
        //just like setPosition(), this assumes there's no rotation or other
        //transformations of that kind
        float x = m_projection(0, 3) / m_projection(0, 0);
        float y = m_projection(1, 3) / m_projection(1, 1);
        m_projection(0, 0) = 2.0f / w;
        m_projection(1, 1) = 2.0f / h;
        setPosition(x, y);
    }
    void TextLine::setColor(float r, float g, float b)
    {
        textColor[0] = r;
        textColor[1] = g;
        textColor[2] = b;
    }


    void TextLine::render(CForge::RenderDevice* pRDev)
    {
        /*
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/

            //     m_pShader->bind();
        m_VertexArray.bind();
        pRDev->activeShader(m_pShader);


        //Shader uniforms
        glActiveTexture(GL_TEXTURE0);
        m_pFont->bind();
        glUniform1i(m_pShader->uniformLocation("glyphMap"), 0);
        glUniform3f(m_pShader->uniformLocation("textColor"), textColor[0], textColor[1], textColor[2]);
        glUniformMatrix4fv(m_pShader->uniformLocation("projection"), 1, GL_FALSE, m_projection.data());

        glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

        m_VertexArray.unbind();

    }

    float TextLine::getTextSize()
    {
        return textSize;
    }
    int TextLine::getTextWidth()
    {
        return m_width;
    }
    int TextLine::computeStringWidth(std::u32string textString)
    {
        return m_pFont->computeStringWidth(textString);
    }

}//name space
