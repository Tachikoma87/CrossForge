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

		void run(void) {
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
			int startFrame = 0;
			int endFrame = 150;
			TestDeformer.resolveCollisionsShapeDeformation(16, 20, startFrame, endFrame, 0);
			//---------------

			M.clear();
		}//run

		void clear(void) {
			//ExampleSceneBase::clear();
		}

		
	protected:
	
	};//ExampleShapeDeformer 

	void exampleShapeDeformer(void) {

		ExampleShapeDeformer Ex;
		Ex.run();
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif