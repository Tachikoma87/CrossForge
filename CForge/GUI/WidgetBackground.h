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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_WIDGETBACKGROUND_H__
#define __CFORGE_WIDGETBACKGROUND_H__

//copied over from the CForge::ScreenQuad
#include "../Graphics/Actors/IRenderableActor.h"
#include "../Graphics/Actors/RenderGroupUtility.h"
#include "../Graphics/Actors/VertexUtility.h"
#include "../Graphics/GLVertexArray.h"
#include "../Graphics/GLBuffer.h"

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

    class CFORGE_API WidgetBackground : public CForge::IRenderableActor/*, public IWidgetBackground*/ {
    public:
        WidgetBackground(BaseWidget* parent, GUI* root);
        ~WidgetBackground(void);
        virtual void initBuffers();

        virtual void setPosition(float x, float y);
        virtual void updatePosition();
        virtual void updateSize(bool initialise = false);
        virtual void setRenderSize(uint32_t w, uint32_t h);   //Size of the window/framebuffer it gets rendered to
        virtual void setColor(float r, float g, float b, float a = -1.0f);
        virtual void setColor(float color[3], float a = -1.0f);

        virtual void clear(void) = 0;
        virtual void release(void);

        // interface method
        virtual void render(class CForge::RenderDevice* pRDev) = 0;

    protected:
        Eigen::Matrix4f m_projection;
        BaseWidget* m_parent;
        float m_color[4] = { 0, 0, 0, 0.5f };

        CForge::GLShader* m_pShader;
        virtual void setBufferData(void);

    };//WidgetBackground

    /*
     Single color background object
     */
    class CFORGE_API WidgetBackgroundColored : public WidgetBackground {
    public:
        WidgetBackgroundColored(BaseWidget* parent, GUI* root);
        ~WidgetBackgroundColored(void);
        void clear(void);
        void render(class CForge::RenderDevice* pRDev);
        //     void release(void);
        // protected:
    };

    /*
     Single color border
     */
    class CFORGE_API WidgetBackgroundBorder : public WidgetBackground {
    public:
        WidgetBackgroundBorder(BaseWidget* parent, GUI* root);
        ~WidgetBackgroundBorder(void);
        void clear(void);
        void updateSize(bool initialise = false);
        void render(class CForge::RenderDevice* pRDev);
        void setLineWidth(float lw);
    private:
        float m_lineWidth;
    };

}//name space
#endif