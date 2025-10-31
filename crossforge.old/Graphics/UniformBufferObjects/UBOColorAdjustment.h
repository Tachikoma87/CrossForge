/*****************************************************************************\
*                                                                           *
* File(s): UBOPostProcessing.h and UBOPostProcessing.cpp                    *
*                                                                           *
* Content: Uniform buffer object for the shader base post processing step   *
*          color adjustment.                                                *
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
#ifndef __CFORGE_UBOPOSTPROCESSING_H__
#define __CFORGE_UBOPOSTPROCESSING_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for the shader base post processing step color adjustment.
	* \ingroup UniformBufferObjects
	*/
	class CFORGE_API UBOColorAdjustment: public CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		UBOColorAdjustment(void);

		/**
		* \brief Destructor
		*/
		~UBOColorAdjustment(void);

		/**
		* \brief Initialization method.
		*/
		void init(void);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the uniform buffer to a specific binding point,
		* 
		* \param[in] BindingPoint Binding point retrieved from the shader.
		*/
		void bind(uint32_t BindingPoint);

		/**
		* \brief Set the contrast value.
		* 
		* \param[in] Contrast The new contrast value.
		*/
		void contrast(float Contrast);

		/**
		* \brief Set the saturation value.
		* 
		* \param[in] Saturation Then new saturation value.
		*/
		void saturation(float Saturation);

		/**
		* \brief The the brightness value.
		* 
		* \param[in] Brightness The new brightness value.
		*/
		void brigthness(float Brightness);

	protected:
		GLBuffer m_Buffer;	///< The OpenGL buffer object.
	};//UBOColorAdjustment

}//name space

#endif 