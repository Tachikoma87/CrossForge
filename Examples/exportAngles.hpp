#ifndef __CFORGE_EXPORTANGLES_HPP__
#define __CFORGE_EXPORTANGLES_HPP__

#include "exampleSceneBase.hpp"
#include <string.h>
using namespace Eigen;
using namespace std;

namespace CForge {

	class ExportAngles {
	public:
		ExportAngles(void) {

		}//Constructor

		~ExportAngles(void) {
			clear();
		}//Destructor

		void run() {
			// load skydome
			T3DMesh<float> M;
			AssetIO::load("Assets/ExampleScenes/ManMulti.fbx", &M);
			//SAssetIO::load("Assets/tmp/WalkingSittingEve2.fbx", &M);
			//SAssetIO::load("Assets/tmp/MuscleMan3.fbx", &M);

			M.sortBones(M.skeletalAnimationCount());
			//M.interpolateKeyframes(816, M.getBone(0), 0);
			M.clearSkeletalAnimations();		
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/Eve3V6+3_3It_IB.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/Eve2V6+3_5It_IB.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/EveV6+3_3It_IB_HalfLearning.dat", &M);
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MM5V6+3_2It_IB.dat", &M); 
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/MM4V6+3_2+3It_IB_DoubleLearning.dat", &M); 
			//AnimationIO::loadSkeletalAnimation("Assets/Animations/TestEricV6+3_3It_I.dat", &M);
			AnimationIO::loadSkeletalAnimation("Assets/Animations/EricNoIGWristElbow.dat", &M);
			int32_t importantBones[6] = { 8,9,10,12,13,14 };


			for (int j = 0; j < 6; j++) {
				string filename = "AnglesOfBone" + std::to_string(importantBones[j]) + ".txt";

				std::ofstream myfile(filename);
				if (myfile.is_open())
				{
					for (int i = 0; i < M.frameCount(0); i++) {
						Eigen::Vector3f Rotation = M.getRotationEuler(i, importantBones[j], 0);
						myfile << Rotation.x() << " , " << Rotation.y() << " , " << Rotation.z() << "\n";
					}
					myfile.close();
				}
				else std::cout << "Unable to open file";

			}

			

			M.clear();
		}//run

		void clear(void) {
			//ExampleSceneBase::clear();
		}


	protected:

	};//ExampleShapeDeformer 

	void exportAngles() {

		ExportAngles Ex;
		Ex.run();
		Ex.clear();

	}//exampleMinimumGraphicsSetup

}

#endif