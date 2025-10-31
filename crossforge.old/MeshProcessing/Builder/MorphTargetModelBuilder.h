/*****************************************************************************\
*                                                                           *
* File(s): MorphTargetBuilder.h and MorphTargetBuilder.cpp                  *
*                                                                           *
* Content: Class to build morph targets for morph target animation.         *
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
#ifndef __CFORGE_MORPHTARGETMODELBUILDER_H__
#define __CFORGE_MORPHTARGETMODELBUILDER_H__

#include "../../Core/CForgeObject.h"
#include "../../AssetIO/T3DMesh.hpp"

namespace CForge {
	/**
	* \brief Class to build morph target animations from static meshes.
	* \ingroup MeshProcessing
	* 
	* See example morph target animation on how this works.
	* \todo Change pass by pointer to pass by reference.
	* \todo Change internal management of data to smart pointer
	*/
	class CFORGE_API MorphTargetModelBuilder: public CForgeObject {
	public: 
		/**
		* \brief Constructor
		*/
		MorphTargetModelBuilder(void);

		/**
		* \brief Destructor
		*/
		~MorphTargetModelBuilder(void);


		/**
		* \brief Initialize
		* 
		* \param[in] pBaseMesh Static mesh that contains the basis for the morph target animation. Usually an averaged or neutral model.
		*/
		void init(T3DMesh<float>* pBaseMesh);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Adds a morph target.
		* 
		* \param[in] pTarget Morph targets. Has to be in full vertex correspondence to the base mesh.
		* \param[in] Name Assign a name to the morph target.
		* \throws CrossForgeException Throws an exception if specified mesh is not in vertex correspondence with the base mesh.
		*/
		void addTarget(T3DMesh<float>* pTarget, std::string Name);

		/**
		* \brief Performs the building step.
		*/
		void build(void);


		/**
		* \brief Retrieve the created morph targets.
		* 
		* \param[out] pMesh Mesh data structure to store the morph targets in.
		*/
		void retrieveMorphTargets(T3DMesh<float>* pMesh);

	protected:

		/**
		* \brief Data structure to store a morph target.
		*/
		struct Target {
			std::string Name;							///< Name of the morph target
			std::vector<Eigen::Vector3f> Positions;		///< Vertex positions
			std::vector<Eigen::Vector3f> Normals;		///< per vertex Normals

			/**
			* \brief Constructor
			*/
			Target() {
				clear();
			}

			/**
			* \brief Destructor
			*/
			~Target() {
				clear();
			}

			/**
			* \brief Clear
			*/
			void clear() {
				Name = "";
				Positions.clear();
				Normals.clear();
			}
		};

		T3DMesh<float> m_BaseMesh;		///< Base mesh.
		std::vector<Target*> m_Targets;	///< Set morph targets.
		std::vector<T3DMesh<float>::MorphTarget*> m_MorphTargets; ///< The final morph targets.

	};//MorphTargetModelBuilder

}//name space

#endif 