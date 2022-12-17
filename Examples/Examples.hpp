/*****************************************************************************\
*                                                                           *
* File(s): Examples.hpp                                               *
*                                                                           *
* Content: Includes all example files and creates functions to conveniently              *
*          start them.      *
*                               *
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
#ifndef __CFORGE_EXAMPLES_HPP__
#define __CFORGE_EXAMPLES_HPP__

#include "exampleSceneBase.hpp"
#include "exampleMinimumGraphicsSetup.hpp"
#include "exampleSkeletalAnimation.hpp"
#include "exampleMorphTargetAnimation.hpp"
#include "exampleSkybox.hpp"
#include "exampleMultiViewport.hpp"
#include "exampleSockets.hpp"
#include "exampleSceneGraph.hpp"

namespace CForge {
	void exampleMinimumGraphicsSetup(void) {
		ExampleMinimumGraphicsSetup Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMinimumGraphicsSetup

	void exampleMorphTargetAnimation(void) {
		ExampleMorphTargetAnimation Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMorphTargetAnimation

	void exampleMultiViewport(void) {
		ExampleMultiViewport Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMultiViewport

	void exampleSceneBase(void) {
		ExampleSceneBase Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMinimumGraphicsSetup

	void exampleSceneGraph(void) {
		ExampleSceneGraph Ex;
		Ex.init();
		Ex.run();
	}//exampleSceneGraph

	void exampleSkeletalAnimation(void) {
		ExampleSkeletalAnimation Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleMinimumGraphicsSetup

	void exampleSkybox(void) {
		ExampleSkybox Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleSkybox

	void exampleSockets(void) {
		ExampleSocket Ex;
		Ex.init();
		Ex.run();
		Ex.clear();
	}//exampleSocketTestScene

}//name space

#endif 