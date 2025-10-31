/*****************************************************************************\
*                                                                           *
* File(s): UBOTextData.h and UBOTextData.cpp                                *
*                                                                           *
* Content: Uniform buffer object required for text rendering.               *
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
#ifndef __CFORGE_UBOTEXTDATA_H__
#define __CFORGE_UBOTEXTDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object required for text rendering.
	* \ingroup UniformBufferObjects
	*/
	class CFORGE_API UBOTextData : public CForgeObject {
	public:
		/**
		* \brief Constructor.
		*/
		UBOTextData(void);

		/**
		* \brief Destructor.
		*/
		~UBOTextData(void);

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
		* \param[in] BindingPoint Binding point obtained from the shader.
		*/
		void bind(uint32_t BindingPoint);

		/**
		* \brief Set the color data.
		* 
		* \param[in] Color The new color data.
		*/
		void color(const Eigen::Vector4f Color);

		/**
		* \brief Set the canvas size.
		* 
		* \param[in] CanvasSize The new canvas size.
		*/
		void canvasSize(Eigen::Vector2f CanvasSize);

		/**
		* \brief Set the text position.
		* 
		* \param[in] TextPosition The new text position.
		*/
		void textPosition(Eigen::Vector2f TextPosition);

		/**
		* \brief Returns the size of the buffer in bytes.
		* 
		* \return Size of the buffer in bytes.
		*/
		uint32_t size(void)const;

	protected:

	private:
		uint32_t m_ColorOffset;			///< Buffer offset of the color data.
		uint32_t m_CanvasSizeOffset;	///< Buffer offset of the canvas size.
		uint32_t m_TextPositionOffset;	///< Buffer offset of the text position.
		
		GLBuffer m_Buffer;				///< OpenGL buffer object.
	};//UBOTextData

}//name space

#endif