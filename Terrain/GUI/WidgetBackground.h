#pragma once

#include <CForge/Graphics/Actors/IRenderableActor.h>
#include <CForge/Graphics/Actors/RenderGroupUtility.h>
#include <CForge/Graphics/Actors/VertexUtility.h>
#include <CForge/Graphics/GLVertexArray.h>
#include <CForge/Graphics/GLBuffer.h>

#include "GUIDefaults.h"

// perhaps not the best name, maybe more like "drawables"

/*
 Abstract class for Widget backgrounds, as there could be multiple (plain color, images, perhaps text).
 */
// class IWidgetBackground {
// public:
//     virtual void init(BackgroundStyle style, CForge::GLShader *pShader = nullptr) = 0;
//     virtual void render(class CForge::RenderDevice* pRDev) = 0;
//     virtual ~IWidgetBackground();
// };

class WidgetBackground : public CForge::IRenderableActor/*, public IWidgetBackground*/ {
	public:
		WidgetBackground(void);
		virtual ~WidgetBackground(void);

		virtual void init(BackgroundStyle style, CForge::GLShader *pShader = nullptr) = 0;
        void setPosition(float x, float y);
        void setSize(float width, float height);
        
		virtual void clear(void) = 0;
		void release(void);

        //durch IRenderableActor bereits vorhanden
		void render(class CForge::RenderDevice* pRDev);

	protected:
        
        float m_x;     //top left point
        float m_y;
        float m_width; 
        float m_height;
        
        BackgroundStyle m_style;

		/*GLVertexArray m_VertexArray;
		GLBuffer m_VertexBuffer;
		GLBuffer m_ElementBuffer;*/
		  CForge::GLShader *m_pShader;
          
        void updatePosition(bool initialise);
        void setBufferData(void);

};//WidgetBackground

/*
 Single color background object
 */
class WidgetBackgroundColored : public WidgetBackground {
public:
    WidgetBackgroundColored (void);
    ~WidgetBackgroundColored (void);
    void init(BackgroundStyle style, CForge::GLShader *pShader = nullptr);
    void clear(void);
//     void release(void);
// protected:
};
