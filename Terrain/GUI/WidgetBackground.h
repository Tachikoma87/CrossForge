#pragma once

//copied over from the CForge::ScreenQuad
#include <CForge/Graphics/Actors/IRenderableActor.h>
#include <CForge/Graphics/Actors/RenderGroupUtility.h>
#include <CForge/Graphics/Actors/VertexUtility.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>

#include "GUIDefaults.h"

// perhaps not the best name, maybe more like "drawables"

/*
 Abstract class for Widget backgrounds, as there could be multiple (plain color, images).
 */

//forwards declaration instead of including it here because otherwise somehow everything
//completely falls apart... It's included in WidgetBackground.cpp instead.
class BaseWidget;

class WidgetBackground : public CForge::IRenderableActor/*, public IWidgetBackground*/ {
    public:
        WidgetBackground(BaseWidget* parent, CForge::GLShader *pShader = nullptr);
        ~WidgetBackground(void);

        virtual void setPosition(float x, float y);
        virtual void updateSize(bool initialise);
        virtual void setColor(float r, float g, float b, float a = -1.0f);
        
        virtual void clear(void) = 0;
        virtual void release(void);

        //durch IRenderableActor bereits vorhanden
        virtual void render(class CForge::RenderDevice* pRDev) = 0;

    protected:
        Eigen::Matrix4f m_projection;
        BaseWidget* m_parent;
        float m_color[4] = {0, 0, 0, 0.5f};

        CForge::GLShader *m_pShader;
        virtual void setBufferData(void);

};//WidgetBackground

/*
 Single color background object
 */
class WidgetBackgroundColored : public WidgetBackground {
public:
    WidgetBackgroundColored (BaseWidget* parent, CForge::GLShader *pShader = nullptr);
    ~WidgetBackgroundColored (void);
    void clear(void);
    void render(class CForge::RenderDevice* pRDev);
//     void release(void);
// protected:
};
