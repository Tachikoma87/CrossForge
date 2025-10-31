/*****************************************************************************\
*                                                                           *
* File(s): UBOBoneData.h and UBOBoneData.cpp                                *
*                                                                           *
* Content: Uniform buffer object for skeletal animated data.                *
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
#ifndef __CFORGE_UBOBONEDATA_H__
#define __CFORGE_UBOBONEDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object skeletal animation (Bones) related data.
	* \ingroup UniformBufferObjects
	* 
	* \todo Rename everything bone to joint
	*/
	class CFORGE_API UBOBoneData : public CForgeObject {
	public:
		/**
		* \brief Constructor.
		*/
		UBOBoneData(void);

		/**
		* \brief Destructor.
		*/
		~UBOBoneData(void);

		/**
		* \brief Initialization method.
		* 
		* \param[in] BoneCount Number of bones.
		*/
		void init(uint32_t BoneCount);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the uniform buffer.
		* 
		* \param[in] BindingPoint The binding point obtained from the shader.
		*/
		void bind(uint32_t BindingPoint);

		/*
		* \brief Set skinning matrix data.
		* 
		* \param[in] Index Joint index.
		* \param[in] SkinningMat The new skinning matrix.
		*/
		void skinningMatrix(uint32_t Index, Eigen::Matrix4f SkinningMat);

		/**
		* \brief Returns the size of the buffer in bytes.
		* 
		* \return Size of the buffer in bytes.
		*/
		uint32_t size(void)const;

	protected:

	private:
		GLBuffer m_Buffer;		///< OpenGL object.
		uint32_t m_BoneCount;	///< Number of joints.
	};//UBOBoneData

}//name space

#endif