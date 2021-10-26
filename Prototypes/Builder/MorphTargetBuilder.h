/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetBuilder.h and MorphTargetBuilder.cpp                  *
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
#ifndef __CFORGE_MORPHTARGETBUILDER_H__
#define __CFORGE_MORPHTARGETBUILDER_H__

#include "../../CForge/AssetIO/T3DMesh.hpp"

namespace CForge {
	class MorphTargetBuilder {
	public: 
		MorphTargetBuilder(void);
		~MorphTargetBuilder(void);

		void init(T3DMesh<float>* pBaseMesh);
		void clear(void);

		void addTarget(T3DMesh<float>* pTarget, std::string Name);
		void build(void);

		void retrieveMorphTargets(T3DMesh<float>* pMesh);

	protected:
		struct Target {
			std::string Name; ///< Name of the morph target
			std::vector<Eigen::Vector3f> Positions; ///< Vertex positions
			std::vector<Eigen::Vector3f> Normals; ///< per vertex Normals
		};

		T3DMesh<float> m_BaseMesh;
		std::vector<Target*> m_Targets;
		std::vector<T3DMesh<float>::MorphTarget*> m_MorphTargets; // the final morph targets

	};//MorphTargetBuilder

}//name space

#endif 