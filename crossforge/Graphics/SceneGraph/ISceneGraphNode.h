/*****************************************************************************\
*                                                                           *
* File(s): ISceneGraphNode.h and ISceneGraphNode.cpp                        *
*                                                                           *
* Content: Base class of all scene graph nodes.                             *
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
#ifndef __CFORGE_ISCENEGRAPHNODE_H__
#define __CFORGE_ISCENEGRAPHNODE_H__


#include <list>
#include "../../Core/CForgeObject.h"
#include "../RenderDevice.h"

namespace CForge {
	/**
	* \brief Interface class of all scene graph nodes. Implements the basic tree operations.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API ISceneGraphNode: public CForgeObject {
	public:

		virtual void update(float FPSScale) = 0;
		virtual void render(RenderDevice *pRDev, const Eigen::Vector3f Position, const Eigen::Quaternionf Rotation, const Eigen::Vector3f Scale) = 0;

		// builds transformation recursively the tree up to root
		virtual void buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale) = 0;

		virtual void init(ISceneGraphNode* pParent = nullptr);
		virtual void clear(void);

		virtual void parent(ISceneGraphNode* pNode);
		virtual ISceneGraphNode* parent(void)const;
		virtual void addChild(ISceneGraphNode* pNode);
		virtual void removeChild(ISceneGraphNode* pNode);
		virtual void removeAllChildren(void);
		virtual bool isChild(const ISceneGraphNode* pNode)const;
		virtual uint32_t childCount(void)const;
		virtual ISceneGraphNode* child(uint32_t Index);

		virtual void enable(bool Update, bool Rendering);
		virtual void enabled(bool* pUpdate, bool* pRendering);

	protected:
		ISceneGraphNode(const std::string ClassName);
		~ISceneGraphNode(void);

		std::list<ISceneGraphNode*> m_Children; ///< child nodes
		ISceneGraphNode* m_pParent; ///< Parent node
		bool m_UpdateEnabled;
		bool m_RenderingEnabled;

	};

}//name space

#endif 