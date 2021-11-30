/*****************************************************************************\
*                                                                           *
* File(s): SGNGeometry.h and SGNGeometery.cpp								*
*                                                                           *
* Content: Class to interact with an MF52 NTC Thermistor by using a basic   *
*          voltage divider circuit.                                         *
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
#ifndef __CFORGE_SGNGEOMETRY_H__
#define __CFORGE_SGNGEOMETRY_H__

#include "ISceneGraphNode.h"
#include "../Actors/IRenderableActor.h"

namespace CForge {

	/**
	* \brief Geometry scene graph node.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_IXPORT SGNGeometry : public ISceneGraphNode {
	public:
		SGNGeometry(void);
		~SGNGeometry(void);

		void init(ISceneGraphNode *pParent, IRenderableActor* pRenderable, Eigen::Vector3f Position = Eigen::Vector3f::Zero(), Eigen::Quaternionf Rotation = Eigen::Quaternionf::Identity(), Eigen::Vector3f Scale = Eigen::Vector3f(1.0f, 1.0f, 1.0f));
		void clear(void);

		void position(Eigen::Vector3f Position);
		void rotation(Eigen::Quaternionf Rotation);
		void scale(Eigen::Vector3f Scale);
		void actor(IRenderableActor* pActor);

		Eigen::Vector3f position(void)const;
		Eigen::Quaternionf rotation(void)const;
		Eigen::Vector3f scale(void)const;
		IRenderableActor* actor(void)const;

		void update(float FPSScale);
		void render(RenderDevice* pRDev, Eigen::Vector3f Position, Eigen::Quaternionf Rotation, Eigen::Vector3f Scale);
		void buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale);
	private:
		Eigen::Vector3f m_Position;
		Eigen::Quaternionf m_Rotation;
		Eigen::Vector3f m_Scale;
		IRenderableActor* m_pRenderable;
	};//SGNGeometry

}//name space

#endif 