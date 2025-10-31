/*****************************************************************************\
*                                                                           *
* File(s): SceneGraph.h and SceneGraph.cpp                                  *
*                                                                           *
* Content: Scene graph.                                                     *
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
#ifndef __CFORGE_SCENEGRAPH_H__
#define __CFORGE_SCENEGRAPH_H__

#include "../RenderDevice.h"
#include "../Camera/VirtualCamera.h"
#include "ISceneGraphNode.h"

namespace CForge {
	/**
	* \brief Scene graph.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API SceneGraph: public CForgeObject {
	public:
		SceneGraph(void);
		~SceneGraph(void);

		void init(ISceneGraphNode *pRoot = nullptr);
		void clear(void);

		void rootNode(ISceneGraphNode* pNode);
		ISceneGraphNode* rootNode(void);

		void update(float FPSScale);
		void render(RenderDevice *pRDev);

	protected:
		ISceneGraphNode* m_pRoot;
	};//SceneGraph

}//name space


#endif 