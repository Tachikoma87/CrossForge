/*****************************************************************************\
*                                                                           *
* File(s): UBOModelData.h and UBOModelData.cpp                       *
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
#ifndef __CFORGE_UBOINSTANCEDDATA_H__
#define __CFORGE_UBOINSTANCEDDATA_H__

#include <crossforge/Graphics/GLBuffer.h>

namespace CForge {
	/**
	* \brief Uniform buffer object for model related data.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API UBOInstancedData : public CForgeObject {
	public:
		UBOInstancedData(void);
		~UBOInstancedData(void);

		void init();
		void clear(void);
		void bind(uint32_t BindingPoint);
		uint32_t size(void)const;

		//void setInstance(uint32_t index, Eigen::Matrix3f rotation, Eigen::Vector3f translation);
		//void setInstance(uint32_t index, Eigen::Matrix4f mat);
		void setInstances(const std::vector<Eigen::Matrix4f>* mats, Eigen::Vector2i range);
		void setInstance(const Eigen::Matrix4f* mat, uint32_t index);
		uint32_t getMaxInstanceCount();

	protected:
		uint32_t m_maxInstanceCount = 0;
		GLBuffer m_Buffer;
		uint32_t m_instanceCount;
	};//UBOModelData


}//name space

#endif