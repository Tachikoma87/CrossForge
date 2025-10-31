/*****************************************************************************\
*                                                                           *
* File(s): UBOMorphTargetData.h and UBOMorphTargetData.cpp                  *
*                                                                           *
* Content: Uniform buffer object for morph target data.                     *
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
#ifndef __CFORGE_UBOMORPHTARGETDATA_H__
#define __CFORGE_UBOMORPHTARGETDATA_H__

#include "../GLBuffer.h"

namespace CForge {
	/**
	* \brief Uniform buffer object for morph target data.
	* \ingroup UniformBufferObjects
	* 
	* \todo Check how the heck this indexation works. It is unclear how many indexes this class can handle. That is to say, make a better description.
	* \todo Check whether dataOffset is deprecated and not needed anymore.
	*/
	class CFORGE_API UBOMorphTargetData: public CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		UBOMorphTargetData(void);

		/**
		* \brief Destructor
		*/
		~UBOMorphTargetData(void);

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
		* \param[in] BindingPoint Binding point retrieved from the buffer.
		*/
		void bind(uint32_t BindingPoint);

		/**
		* \brief Returns the size of the buffer in bytes.
		* 
		* \return Size of buffer in bytes.
		*/
		uint32_t size(void)const;

		/**
		* \brief Set the morph target parameters.
		* 
		* \param[in] Index Buffer slot to write at.
		* \param[in] MorphTargetID The morph target id.
		* \param[in] Strength The activation strength.
		*/
		void setMorphTargetParam(uint32_t Index, uint32_t MorphTargetID, float Strength);

		/**
		* \brief Sets the number of active morph targets, i.e. active slots.
		* 
		* \param[in] Count Number of active slots.
		*/
		void activeMorphTargets(int32_t Count);

		/**
		* \brief Sets the data offset between each morph target displacement data. 
		* 
		* \param[in] DataOffset Data offset.
		* \note This method may be deprecated since a data offset is only required when using a one dimensional buffer, but we are using a 2d texture now.
		*/
		void dataOffset(int32_t DataOffset);

	protected:
		GLBuffer m_Buffer;			///< The OpenGL buffer object.

		uint32_t m_DataOffset;				///< Buffer offset of data offset data.
		uint32_t m_ActivationIDsOffset;		///< Buffer offset of activation ids.
		uint32_t m_ActivationStrengthsOffset; ///< Buffer offset of activation strengths.

	};//UBOMorphTargetData
}//name space

#endif 