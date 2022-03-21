#include "Font.h"
#include "GUIDefaults.h"
#include <stdio.h>

#include <string>
#include <math.h>

#include <glad/glad.h>
#include "CForge/Graphics/Shader/GLShader.h"
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>

FontFace::FontFace()
{
    //initialise Freetype
    int error;
    error = FT_Init_FreeType(&library);
    if (error) {
        //Error occured loading the library
        //TODO: figure out how to exit the program
        printf("Error loading freetype\n");
    }


    FontStyle defaults;
    error = FT_New_Face( library, defaults.FileName.c_str(), 0, &face );
    if ( error == FT_Err_Unknown_File_Format ) {
        //File opened but not a supported font format
        printf("Error reading Font %s: unsupported format\n", defaults.FileName.c_str());
    } else if ( error ) {
        //File not found or unable to open or whatever
        printf("Error opening Font %s\n", defaults.FileName.c_str());
    }
    FT_Set_Pixel_Sizes(face, 0, defaults.PixelSize);

    prepareCharMap();

}
FontFace::~FontFace() {
    for (auto x : glyphs) {
        delete x.second.bitmap;
    }
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

int FontFace::renderString(std::u32string text, CForge::GLBuffer* vbo, CForge::GLVertexArray* vao)
{
    //Prepare the buffer data
    //Since GL_QUADS is not officially allowed for glDrawArrays
    //in the OpenGL 4 specification, triangles will be used instead,
    //resulting in 6 vertices per character with 4 components each,
    //2 position and 2 tex coords.
    const int numVertices = text.length() * 6;
    GLfloat *vertices = new GLfloat[numVertices * 4];


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
        float ypos = -glyph.bitmapOffset.y;
        float w = glyph.bitmapSize.x, h = glyph.bitmapSize.y; //for easier access
        vertices[vertexIndex] = xpos;
        vertices[vertexIndex + 1] = ypos + w;
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

    //find out why it crashes
    GLenum errorCode;

    vao->bind();
    errorCode = glGetError();
    assert (errorCode == 0);
    vbo->init(CForge::GLBuffer::BTYPE_VERTEX, CForge::GLBuffer::BUSAGE_STATIC_DRAW, vertices, sizeof(float)*numVertices*4);
    vbo->bind();
    errorCode = glGetError();
    assert (errorCode == 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    errorCode = glGetError();
    assert (errorCode == 0);
   
    errorCode = glGetError();
    assert (errorCode == 0);
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

void FontFace::prepareCharMap()
{
    //for now, use the first ~220 codepoints as characters for the map
    //could be changed to something more specific in the future

    //determine the size of the map necessary to fit all glyphs
    //
    //1) load all glyphs and determine the total width and height of the tallest glyph.
    //   Then use 2*sqrt(width*height) as a crude approximation to get a
    //   roughly square aspect ratio.
    int totalWidth = 0;
    int tallestGlyph = 0;
    for (int i = 32; i < 256; i++) {
        glyph_t character = getGlyph(i);
        totalWidth += character.bitmapSize.x;
        if (tallestGlyph < character.bitmapSize.y) tallestGlyph = character.bitmapSize.y;
    }
    const int targetWidth = int(sqrt(tallestGlyph * totalWidth));

    //2) pack the map theoretically first to get more accurate
    //   width and height numbers before writing the buffer
    //    and fill in the offset in the glyph_t struct
    int maxRowWidth = 0;
    int totalHeight = 0;
    int currentRowWidth = 0, currentRowHeight = 0;
    for (int i = 32; i < 256; i++) {
        glyph_t character = getGlyph(i);
        int newCurrentRowWidth = currentRowWidth + character.bitmapSize.x;
        if (newCurrentRowWidth < targetWidth) {
            //glyph fits into the row
            glyphs[i].charMapOffset.x = currentRowWidth;
            currentRowWidth = newCurrentRowWidth;
            if (character.bitmapSize.y > currentRowHeight)
                currentRowHeight = character.bitmapSize.y;
        } else {
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
    uint8_t *mapBuffer = new uint8_t[numPixels];

    //2) make sure it's filled with 0 (transparent)
    for (int i = 0; i < numPixels; i++) {
        mapBuffer[i] = 0;
    }

    //3) write the bitmap data
    currentRowWidth = 0;
    currentRowHeight = 0;
    for (int i = 32; i < 256; i++) {
        glyph_t character = getGlyph(i);
        for (int y = 0; y < character.bitmapSize.y; y++) {
            for (int x = 0; x < character.bitmapSize.x; x++) {
                const int j = (character.charMapOffset.y + y) * maxRowWidth + (x + character.charMapOffset.x);
                mapBuffer[j] = character.bitmap[y * character.bitmapSize.x + x];
            }
        }
    }
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

    //find out why it crashes
    GLenum errorCode;

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
    errorCode = glGetError();
    assert (errorCode == 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    errorCode = glGetError();
    assert (errorCode == 0);

    delete[] mapBuffer;
}

glyph_t FontFace::getGlyph(char32_t character)
{
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

void FontFace::bind()
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}


TextLine::TextLine()
{
    //use init instead
}

void TextLine::init(std::u32string text, FontFace* pFontFace, CForge::GLShader* pShader)
{
    m_pFont = pFontFace;
    m_pShader = pShader;
    m_VertexArray.init();
    m_numVertices = 0;
    setText(text);
    m_projection = Eigen::Matrix4f::Identity();
    float scale_x, scale_y;
    scale_x = scale_y = 1.0f/720.0f;
    m_projection(0,0) = scale_x;
    m_projection(1,1) = scale_y;
    m_projection(0,3) = scale_x * 100;
    m_projection(1,3) = scale_y * 100;
}

void TextLine::setText(std::u32string text)
{
    //TODO: clean up old buffers if necessary?
    m_numVertices = m_pFont->renderString(text, &m_VertexBuffer, &m_VertexArray);
}

void TextLine::render(CForge::RenderDevice* pRDev)
{
    //find out why it crashes
    GLenum errorCode;

    glEnable(GL_BLEND);
        errorCode = glGetError();
        assert (errorCode == 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        errorCode = glGetError();
        assert (errorCode == 0);

//     m_pShader->bind();
    m_VertexArray.bind();
        errorCode = glGetError();
        assert(errorCode == 0);
    pRDev->activeShader(m_pShader);
        errorCode = glGetError();
        assert (errorCode == 0);
   

    //Shader uniforms
    glActiveTexture(GL_TEXTURE0);
        errorCode = glGetError();
        assert (errorCode == 0);
    m_pFont->bind();
        errorCode = glGetError();
        assert (errorCode == 0);
    glUniform1i(m_pShader->uniformLocation("glyphMap"), 0);
        errorCode = glGetError();
        assert (errorCode == 0);
    glUniform3f(m_pShader->uniformLocation("textColor"), 1.0f, 1.0f, 1.0f);
        errorCode = glGetError();
        assert (errorCode == 0);
    glUniformMatrix4fv(m_pShader->uniformLocation("projection"), 1, GL_FALSE, m_projection.data());
        errorCode = glGetError();
        assert (errorCode == 0);

    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);


    m_VertexArray.unbind();
}
