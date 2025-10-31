/*****************************************************************************\
*                                                                           *
* File(s): SkyboxActor.h and SkyboxActor.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_SKYBOXACTOR_H__
#define __CFORGE_SKYBOXACTOR_H__

#include "IRenderableActor.h"
#include "../Textures/GLCubemap.h"
#include "../UniformBufferObjects/UBOColorAdjustment.h"

namespace CForge {
	class CFORGE_API SkyboxActor : public IRenderableActor {
	public:
		SkyboxActor(void);
		~SkyboxActor(void);

		void init(std::string Right, std::string Left, std::string Top, std::string Bottom, std::string Back, std::string Front);
		void clear(void);

		void render(class RenderDevice* pRDev, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale);

		void saturation(float Saturation);
		void brightness(float Brightness);
		void contrast(float Contrast);

		float saturation(void)const;
		float brightness(void)const;
		float contrast(void)const;
	protected:
		GLCubemap m_Cubemap;
		UBOColorAdjustment m_ColorAdjustUBO;

		float m_Saturation;
		float m_Brightness;
		float m_Contrast;
	};//SkyboxActor

}//name space


#endif 