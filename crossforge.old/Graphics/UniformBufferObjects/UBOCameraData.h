/*****************************************************************************\
*                                                                           *
* File(s): UBOCameraData.h and UBOCameraData.cpp                            *
*                                                                           *
* Content: Uniform buffer object for camera related data.                   *
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
#ifndef __CFORGE_UBOCAMERADATA_H__
#define __CFORGE_UBOCAMERADATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for camera related data.
	* \ingroup UniformBufferObjects
	*/
	class CFORGE_API UBOCameraData: public CForgeObject {
	public:

		/**
		* \brief Constructor.
		*/
		UBOCameraData(void);

		/**
		* \brief Destructor.
		*/
		~UBOCameraData(void);

		/**
		* \brief Initialization method.
		*/
		void init(void);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the buffer.
		* 
		* \param[in] BindingPoint Binding point obtained from the shader.
		*/
		void bind(uint32_t BindingPoint);

		/**
		* \brief Set view matrix.
		* 
		* \param[in] Mat New view matrix.
		*/
		void viewMatrix(const Eigen::Matrix4f Mat);

		/**
		* \brief Set projection matrix.
		* 
		* \param[in] Mat The new projection matrix.
		*/
		void projectionMatrix(const Eigen::Matrix4f Mat);

		/**
		* \brief Set camera position.
		* 
		* \param[in] Pos New camera position.
		*/
		void position(const Eigen::Vector3f Pos);

		/**
		* \brief Returns the size of the buffer in bytes.
		* 
		* \return Size of the buffer in bytes.
		*/
		uint32_t size(void)const;

	protected:
		
	private:
		uint32_t m_ViewMatrixOffset;		///< View matrix offset.
		uint32_t m_ProjectionMatrixOffset;	///< Projection matrix offset.
		uint32_t m_Positionoffset;			///< Position data offset.

		GLBuffer m_Buffer;	///< OpenGL buffer object.

	};//UBOCameraData

}//name space

#endif