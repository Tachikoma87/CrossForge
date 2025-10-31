/*****************************************************************************\
*                                                                           *
* File(s): UBOMaterialData.h and UBOMaterialData.cpp                        *
*                                                                           *
* Content: Uniform buffer object for material related data.                 *
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
#ifndef __CFORGE_UBOMATERIALDATA_H__
#define __CFORGE_UBOMATERIALDATA_H__


#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for material related data.
	* \ingroup UniformBufferObjects
	* 
	* \todo Add method to update from a material definition (Render material or T3DMesh<float>::Material.
	*/
	class CFORGE_API UBOMaterialData: public CForgeObject {
	public:
		/**
		* \brief Constructor.
		*/
		UBOMaterialData(void);

		/**
		* \brief Destructor.
		*/
		~UBOMaterialData(void);

		/**
		* \brief Initialization method.
		*/
		void init(void);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the buffer to a specific binding point.
		* 
		* \param[in] BindingPoint Binding point retrieved from the shader.
		*/
		void bind(uint32_t BindingPoint);

		/**
		* \brief Returns the size of the buffer in bytes.
		* 
		* \return Size of buffer in bytes.
		*/
		uint32_t size(void)const;

		/**
		* \brief Set the color values.
		* 
		* \param[in] Color The new color values.
		*/
		void color(Eigen::Vector4f Color);

		/**
		* \brief Set the metallic value.
		* 
		* \param[in] Metallic The new values for metallic.
		*/
		void metallic(float Metallic);

		/**
		* \brief Set the roughness value.
		* 
		* \param[in] Roughness The new roughness value.
		*/
		void roughness(float Roughness);

		/**
		* \brief Set the ambient occlusion value.
		* 
		* \param[in] AO The new ambient occlusion value.
		*/
		void ambientOcclusion(float AO);

	protected:
		uint32_t m_ColorOffset;			///< Buffer offset of the color data.
		uint32_t m_MetallicOffset;		///< Buffer offset of the metallic data.
		uint32_t m_RoughnessOffset;		///< Buffer offset of the roughness data.
		uint32_t m_AmbientOcclusionOffset;	///< Buffer offset of the ambient occlusion data.

		GLBuffer m_Buffer;			///< OpenGL buffer object.
	};//UBOMaterialData

}//name space


#endif