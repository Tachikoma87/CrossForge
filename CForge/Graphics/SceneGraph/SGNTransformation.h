/*****************************************************************************\
*                                                                           *
* File(s): SGNTransformation.h and SGNGeometry.h							*
*                                                                           *
* Content: Class to interact with an MF52 NTC Thermistor by using a basic   *
*          voltage divider circuit.                                         *
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
#ifndef __CFORGE_SGNTRANSFORMATION_H__
#define __CFORGE_SGNTRANSFORMATION_H__

#include "ISceneGraphNode.h"

namespace CForge {
	/**
	* \brief Scene graph node that provides affine transformation. Can also be used to create simple continuous animations.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API SGNTransformation : public ISceneGraphNode {
	public:
		SGNTransformation(void);
		~SGNTransformation(void);

		void init(ISceneGraphNode *pParent, 
			Eigen::Vector3f Translation = Eigen::Vector3f::Zero(), 
			Eigen::Quaternionf Rotation = Eigen::Quaternionf::Identity(), 
			Eigen::Vector3f Scale = Eigen::Vector3f::Ones(), 
			Eigen::Vector3f TranslationDelta = Eigen::Vector3f::Zero(), 
			Eigen::Quaternionf RotationDelta = Eigen::Quaternionf::Identity(), 
			Eigen::Vector3f ScaleDelta = Eigen::Vector3f::Zero());
		void clear(void);

		void translation(Eigen::Vector3f Translation);
		void rotation(Eigen::Quaternionf Rotation);
		void scale(Eigen::Vector3f Scale);
		void translationDelta(Eigen::Vector3f TranslationDelta);
		void rotationDelta(Eigen::Quaternionf RotationDelta);
		void scaleDelta(Eigen::Vector3f ScaleDelta);

		Eigen::Vector3f translation(void)const;
		Eigen::Quaternionf rotation(void)const;
		Eigen::Vector3f scale(void)const;
		Eigen::Vector3f translationDelta(void)const;
		Eigen::Quaternionf rotationDelta(void)const;
		Eigen::Vector3f scaleDelta(void)const;

		void update(float FPSScale);
		void render(RenderDevice* pRDev, const Eigen::Vector3f Translation, const Eigen::Quaternionf Rotation, const Eigen::Vector3f Scale);
		void buildTansformation(Eigen::Vector3f* pPosition, Eigen::Quaternionf* pRotation, Eigen::Vector3f* pScale);

	private:
		Eigen::Vector3f m_Translation;
		Eigen::Quaternionf m_Rotation;
		Eigen::Vector3f m_Scale;
		Eigen::Vector3f m_TranslationDelta;
		Eigen::Quaternionf m_RotationDelta;
		Eigen::Vector3f m_ScaleDelta;
	};//SGNTransformation

}//name space


#endif 