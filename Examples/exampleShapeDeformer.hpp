#ifndef __CFORGE_EXAMPLESHAPEDEFORMER_HPP__
#define __CFORGE_EXAMPLESHAPEDEFORMER_HPP__

#include "exampleSceneBase.hpp"
#include "../Prototypes/MeshProcessing/ShapeDeformer.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	class ExampleShapeDeformer{
	public:
		ExampleShapeDeformer(void) {

		}//Constructor

		~ExampleShapeDeformer(void) {
			clear();
		}//Destructor

		void run(int32_t method, int32_t startFrame, int32_t endFrame) {
			// load skydome
			T3DMesh<float> M;
			// initialize skeletal actor (Eric) and its animation controller
			SAssetIO::load("Assets/tmp/MuscleManSittingWalking.glb", &M);
			//SAssetIO::load("Assets/tmp/WalkingSittingEve.glb", &M);
			M.sortBones(1);
			M.mergeRedundantVertices();
			//-------- Deformation Part
			ShapeDeformer TestDeformer;
			TestDeformer.init(&M);
			TestDeformer.resolveCollisionsShapeDeformation(16, 20, startFrame, endFrame, 0, method);
			//---------------

			M.clear();
		}//run

		void clear(void) {
			//ExampleSceneBase::clear();
		}

		
	protected:
	
	};//ExampleShapeDeformer 

	void exampleShapeDeformer(int32_t method, int32_t startFrame, int32_t endFrame) {

		ExampleShapeDeformer Ex;
		Ex.run(method, startFrame, endFrame);
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif