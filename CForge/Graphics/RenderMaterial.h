/*****************************************************************************\
*                                                                           *
* File(s): RenderMaterial.h and RenderMaterial.cpp                                      *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_RENDERMATERIAL_H__
#define __CFORGE_RENDERMATERIAL_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T3DMesh.hpp"
#include "GLTexture2D.h"


namespace CForge {
	/**
	* \brief Material definition that can be used for rendering objects.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT RenderMaterial: public CForgeObject {
	public:
		RenderMaterial(void);
		~RenderMaterial(void);

		void init(const T3DMesh<float>::Material *pMat);
		void clear(void);

		GLTexture2D* albedoMap(void)const;
		GLTexture2D* normalMap(void)const;
		GLTexture2D* depthMap(void)const;

		const Eigen::Vector4f color(void)const;
		const float metallic(void)const;
		const float roughness(void)const;
		const float ambientOcclusion(void)const;

		void color(const Eigen::Vector4f Color);
		void metallic(float Metallic);
		void roughness(float Roughness);
		void ambientOcclusion(float Ao);

	protected:

	private:
		GLTexture2D* m_pAlbedoMap;
		GLTexture2D* m_pNormalMap;
		GLTexture2D* m_pDepthMap;
		Eigen::Vector4f m_Color;

		float m_Metallic;
		float m_Roughness;
		float m_AmbientOcclusion;
		
	};//RenderMaterial

}//name space

#endif 