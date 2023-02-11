/*****************************************************************************\
*                                                                           *
* File(s): LineOfText.h and LineOfText.cpp                                      *
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
#ifndef __CFORGE_LINEOFTEXT_H__
#define __CFORGE_LINEOFTEXT_H__

#include <CForge/Graphics/RenderDevice.h>
#include "FontFace.h"
#include "UBOTextData.h"

namespace CForge {

    /**
     * \brief A class representing a single line of text for graphical display.
     *
     * \ingroup GUI
     */
    class LineOfText {
    public:
        /** \brief Use \ref init() to initialise the object. */
        LineOfText();
        ~LineOfText();

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


        

        void text(std::string Text);

        /** \brief Changes the text color. */
        void color(float r, float g, float b, float a = 1.0f);
        void color(const Eigen::Vector4f Color);


        /** \brief Sets the text position in window space.
         *  \sa CForge::mouseEventInfo */
        void position(float x, float y);

        /**
         * \brief Pass the size of the render window.
         *
         * Because OpenGL shaders operate in a coordinate space from -1 to 1
         * but the GUI uses window coordinates from 0 to the window's pixel
         * size, we need to convert the coordinates. For that to work, the
         * window dimensions need to be known.
         */
        void canvasSize(uint32_t Width, uint32_t Height);   //Size of the window/framebuffer it gets rendered to

        std::string text(void)const;
        Eigen::Vector4f color(void)const;
        Eigen::Vector2f position(void)const;
        Eigen::Vector2i canvasSize(void)const;
        const FontFace* fontFace(void)const;
        
        /** \brief Getter for the FontFace's text size in pixels. */
        float textSize();

        /** \brief Getter for the currently loaded strings display width in pixels. */
        int textWidth();

        /** \brief Draw the text line to the screen using OpenGL.
         *  \param[in] pRDev Pointer to the CForge::RenderDevice used for drawing the GUI/Scene. */
        void render(CForge::RenderDevice* pRDev);

        int32_t computeStringWidth(const std::string Text);

    protected:
        /** \brief Sets the text that should be displayed.
         *  \param[in] text 32-Bit Unicode string that should be loaded. */
        void text(std::u32string Text);

        /** \brief Proxy for FontFace::computeStringWidth. Should be avoided in favour of the former. */
        int computeStringWidth(std::u32string textString);  //deprecated, better access the font directly if possible


        FontFace* m_pFont;                      ///< Pointer to the currently used FontFace object.
        float m_TextSize;                       ///< The fonts text size in pixels.
       
        int m_NumVertices;                      ///< The number of vertices of the current string.
        int m_Width;                            ///< The pixel width of the current string. 
        Eigen::Vector4f m_TextColor;            ///< The current text colour.
        Eigen::Vector2f m_TextPosition;
        Eigen::Vector2i m_CanvasSize;
        std::string m_Text;

        UBOTextData m_TextUBO;
        CForge::GLShader* m_pShader;            ///< Pointer for the text shader.
        CForge::GLBuffer m_VertexBuffer;        ///< The vertex buffer to hold the text's vertex data.
        CForge::GLVertexArray m_VertexArray;    ///< A vertex array wrapping the vertex buffer.
    };

}//name space

#endif 