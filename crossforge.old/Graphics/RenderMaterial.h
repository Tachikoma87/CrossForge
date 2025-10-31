/*****************************************************************************\
*                                                                           *
* File(s): RenderMaterial.h and RenderMaterial.cpp                          *
*                                                                           *
* Content: Material definition that can be used for rendering objects.      *
*                                                                           *
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
#ifndef __CFORGE_RENDERMATERIAL_H__
#define __CFORGE_RENDERMATERIAL_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T3DMesh.hpp"
#include "Textures/GLTexture2D.h"


namespace CForge {
	/**
	* \brief Material definition that can be used for rendering objects.
	* \ingroup Graphics
	* 
	* \todo Change internal handling to smart pointers.
	* \todo Change return values of methods to smart pointers.
	* \todo Add setter for the textures.
	*/
	class CFORGE_API RenderMaterial: public CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		RenderMaterial(void);

		/**
		* \brief Destructor
		*/
		~RenderMaterial(void);


		/**
		* \brief Initializes the class from a T3DMesh material definition.
		* 
		* \param[in] pMat Material definition.
		*/
		void init(const T3DMesh<float>::Material *pMat);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Getter of the albedo map.
		* 
		* \return Albedo map.
		*/
		GLTexture2D* albedoMap(void)const;

		/**
		* \brief Getter of the normal map.
		* 
		* \return Normal map.
		*/
		GLTexture2D* normalMap(void)const;

		/**
		* \brief Getter of the depth map.
		* 
		* \return Depth map.
		*/
		GLTexture2D* depthMap(void)const;

		/**
		* \brief Getter of the color value.
		* 
		* \return RGBA-color vector.
		*/
		const Eigen::Vector4f color(void)const;

		/**
		* \brief Getter of the metallic value.
		* 
		* \return Metallic value.
		*/
		const float metallic(void)const;

		/**
		* \brief Getter of the roughness value.
		* 
		* \return Roughness value.
		*/
		const float roughness(void)const;

		/**
		* \brief Getter of the ambient occlusion value.
		* 
		* \return Ambient occlusion value.
		*/
		const float ambientOcclusion(void)const;

		/**
		* \brief Setter of the color value.
		* 
		* \param[in] Color new RGBA-color value.
		*/
		void color(const Eigen::Vector4f Color);

		/**
		* \brief Setter of the metallic value.
		* 
		* \param[in] Metallic New metallic value.
		*/
		void metallic(float Metallic);

		/**
		* \brief Setter of the roughness value.
		* 
		* \param[in] Roughness New roughness value.
		*/
		void roughness(float Roughness);

		/**
		* \brief Setter of the ambient occlusion value.
		* 
		* \param[in] Ao Ambient occlusion value.
		*/
		void ambientOcclusion(float Ao);

	protected:

		GLTexture2D* m_pAlbedoMap;	///< Albedo texture.
		GLTexture2D* m_pNormalMap;	///< Normal texture.
		GLTexture2D* m_pDepthMap;	///< Depth texture.
		Eigen::Vector4f m_Color;	///< RGBA color value.

		float m_Metallic;			///< Metallic value.
		float m_Roughness;			///< Roughness value.
		float m_AmbientOcclusion;	///< Ambient occlusion value.
		
	};//RenderMaterial

}//name space

#endif 