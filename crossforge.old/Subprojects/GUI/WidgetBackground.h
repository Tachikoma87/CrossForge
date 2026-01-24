/*****************************************************************************\
*                                                                           *
* File(s): WidgetBackground.h and WidgetBackground.cpp                                      *
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
#ifndef __CFORGE_WIDGETBACKGROUND_H__
#define __CFORGE_WIDGETBACKGROUND_H__

//copied over from the CForge::ScreenQuad
#include <CForge/Graphics/Actors/IRenderableActor.h>
#include <CForge/Graphics/Actors/RenderGroupUtility.h>
#include <CForge/Graphics/Actors/VertexUtility.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>

#include "GUI.h"
#include "GUIDefaults.h"

namespace CForge {

    /*
     Abstract class for Widget backgrounds, as there could be multiple (plain color, images).
     */

     //forwards declaration instead of including it here because otherwise somehow everything
     //completely falls apart... It's included in WidgetBackground.cpp instead.
    class GUI;
    class BaseWidget;

    /**
     * \brief Common interface class for different widget background types.
     *
     * Widgets don't directly interact with OpenGL for drawing backgrounds
     * or borders. Instead, these WidgetBackground classes abstract that.
     *
     * This is the interface class that holds common methods used by all
     * currently implemented "background" types (solid color background
     * and solid color border).
     *
     * \ingroup GUI
     */
    class WidgetBackground : public CForge::IRenderableActor/*, public IWidgetBackground*/ {
    public:

        /**
         * \brief Initializes the WidgetBackground.
         *
         * Gets the dimensions from the parent widget and the background
         * shader from the GUI main class.
         *
         * \param[in] parent    The style object of the font to load (includes path of fot file and the requested size).
         * \param[in] root      The FreeType library handle to use. One global instance can load multiple font faces.
         */
        WidgetBackground(BaseWidget* parent, GUI* root);
        ~WidgetBackground(void);

        /** \brief Initialises the vertex buffer of the background. */
        virtual void initBuffers();

        /** \brief Sets the on screen position of the background in window space.
         *  Usually synced with the widget through WidgetBackground::updatePosition */
        virtual void setPosition(float x, float y);

        /** \brief Synchronise the position of the background with assigned parent widget. */
        virtual void updatePosition();

        /** \brief Synchronise the size of the background with assigned parent widget.
         *  \param[in] initialise Whether the vertex buffer should be initialised or already exists. */
        virtual void updateSize(bool initialise = false);

        /**
         * \brief Pass the size of the render window.
         *
         * Because OpenGL shaders operate in a coordiate space from -1 to 1
         * but the GUI uses window coordinates from 0 to the window's pixel
         * size, we need to convert the coordinates. For that to work, the
         * window dimensions need to be known.
         */
        virtual void setRenderSize(uint32_t w, uint32_t h);   //Size of the window/framebuffer it gets rendered to

        /** \brief Set the background colour. */
        virtual void setColor(float r, float g, float b, float a = -1.0f);

        /** \brief Set the background colour. For convenience, RGB can be passed as array. */
        virtual void setColor(float color[3], float a = -1.0f);

        /** \brief Release and clean up any resources used. */
        virtual void clear(void) = 0;

        /** \brief Explicitly destroy and release this instance. */
        virtual void release(void);

        /** \brief Draw the background to the screen using OpenGL.
         *  \param[in] pRDev Pointer to the CForge::RenderDevice used for drawing the GUI/Scene. */
        virtual void render(class CForge::RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) = 0;

    protected:
        Eigen::Matrix4f m_projection;           ///< The projection matrix used to correctly scale and position the text on screen.
        BaseWidget* m_parent;                   ///< The widget a background instance is assigned to.
        float m_color[4] = { 0, 0, 0, 0.5f };   ///< The backgrounds colour.

        CForge::GLShader* m_pShader;            ///< The backgrounds used shader to draw it.

        /** \brief Fill the background's vertex buffer. */
        virtual void setBufferData(void);

    };//WidgetBackground

    /**
     * \brief A single solid colour widget background.
     *
     * \ingroup GUI
     */
    class WidgetBackgroundColored : public WidgetBackground {
    public:
        WidgetBackgroundColored(BaseWidget* parent, GUI* root);
        ~WidgetBackgroundColored(void);
        void clear(void);
        void render(class CForge::RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);
        //     void release(void);
        // protected:
    };

    /**
     * \brief A single colour widget border.
     *
     * \ingroup GUI
     */
    class CFORGE_API WidgetBackgroundBorder : public WidgetBackground {
    public:
        WidgetBackgroundBorder(BaseWidget* parent, GUI* root);
        ~WidgetBackgroundBorder(void);
        void clear(void);
        void updateSize(bool initialise = false);
        void render(class CForge::RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

        /** \brief Set the border's line width.
         *  \param[in] lw The new line width in pixels that should be used. */
        void setLineWidth(float lw);
    private:
        float m_lineWidth;  ///< The border's line width.
    };

}//name space
#endif
