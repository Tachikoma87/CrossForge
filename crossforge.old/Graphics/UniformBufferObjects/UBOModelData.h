/*****************************************************************************\
*                                                                           *
* File(s): UBOModelData.h and UBOModelData.cpp                              *
*                                                                           *
* Content: Uniform buffer object for model related data.                    *
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
#ifndef __CFORGE_UBOMODELDATA_H__
#define __CFORGE_UBOMODELDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for model related data.
	* \ingroup UniformBufferObject
	*/
	class CFORGE_API UBOModelData: public CForgeObject {
	public:
		/**
		* \brief Constructor.
		*/
		UBOModelData(void);

		/**
		* \brief Destructor
		*/
		~UBOModelData(void);

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
		* \return Size of the buffer in bytes.
		*/
		uint32_t size(void)const;

		/**
		* \brief Set values of the model matrix.
		* 
		* \param[in] Mat New values for the model matrix.
		*/
		void modelMatrix(Eigen::Matrix4f Mat);

		/**
		* \brief Set values of the normal matrix.
		* 
		* \param[in] Mat New values for the normal matrix.
		*/
		void normalMatrix(Eigen::Matrix4f Mat);

	protected:
		GLBuffer m_Buffer;				///< OpenGL buffer object.
		uint32_t m_ModelMatrixOffset;	///< Buffer offset of the model data.
		uint32_t m_NormalMatrixOffset;	///< Buffer offset of the normal data.
	};//UBOModelData


}//name space

#endif