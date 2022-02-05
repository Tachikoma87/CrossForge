/*****************************************************************************\
*                                                                           *
* File(s): RenderGroupUtility.h and RenderGroupUtility.cpp                    *
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
#ifndef __CFORGE_RENDERGROUPUTILITY_H__
#define __CFORGE_RENDERGROUPUTILITY_H__

#include "../../Core/CForgeObject.h"
#include "../RenderMaterial.h"
#include "../Shader/GLShader.h"
#include "../../AssetIO/T2DImage.hpp"

namespace CForge {
	/**
	* \brief Groups faces according to their used material and shader. Provides these information for rendering.
	*
	* \todo Do full documentation
	*/
	class CFORGE_API RenderGroupUtility: public CForgeObject {
	public:
		struct RenderGroup {
			Eigen::Vector2i Range; ///< From index, to index
			RenderMaterial Material; ///< Material data 
			GLShader *pShader;

			RenderGroup(void) {
				pShader = nullptr;
			}

			~RenderGroup(void) {
				pShader = nullptr;
			}
		};//RenderGroup

		RenderGroupUtility(void);
		~RenderGroupUtility(void);

		void init(const T3DMesh<float>* pMesh, void** ppBuffer = nullptr, uint32_t* pBufferSize = nullptr);
		void clear(void);
		void buildIndexArray(const T3DMesh<float>* pMesh, void** ppBuffer, uint32_t* pBufferSize);

		std::vector<RenderGroup*> renderGroups(void);
		uint32_t renderGroupCount(void)const;

	protected:

	private:
		std::vector<RenderGroup*> m_RenderGroups;

	};//RenderGroupUtility

}//name space


#endif