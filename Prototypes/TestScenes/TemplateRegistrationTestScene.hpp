#ifndef __CFORGE_TEMPLATEREGISTRATIONTESTSCENE_HPP__
#define __CFORGE_TEMPLATEREGISTRATIONTESTSCENE_HPP__

#include "../TemplateRegistration/Viewer/TempRegViewer.h"

using namespace Eigen;
using namespace TempReg;

namespace CForge {

	void tempRegTestScene(void) {

		TempRegViewer TRegViewer;
		TRegViewer.init();

		// temporary - just for testing purposes:
		//TRegViewer.initTemplateFromFile(TemplateFitter::GeometryType::MESH, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI!
		TRegViewer.initTemplateFromFile(TemplateFitter::GeometryType::MESH, "Assets/ExampleScenes/TempReg/Template_CoarseFitTest.obj"); //TODO: move to GUI!
		//TRegViewer.initTargetFromFile(TemplateFitter::GeometryType::MESH, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!
		//TRegViewer.initTargetFromFile(TemplateFitter::GeometryType::POINTCLOUD, "Assets/ExampleScenes/TempReg/Template.obj"); //TODO: move to GUI! --- ".../Template.obj" used for testing purposes, change to Target.obj!
		TRegViewer.initTargetFromFile(TemplateFitter::GeometryType::MESH, "Assets/ExampleScenes/TempReg/Target_CoarseFitTest_Small.obj");
		//TRegViewer.initTargetFromFile(TemplateFitter::GeometryType::MESH, "Assets/ExampleScenes/TempReg/Target_CoarseFitTest_Large.obj");

		// main rendering loop
		while (!TRegViewer.shutdown()) {
			TRegViewer.processInput();
			TRegViewer.render(); // render interface (viewports and GUI)
			TRegViewer.finishFrame();
		}//while[main loop]

		TRegViewer.releaseShaderManager();

	} //tempRegTestScene
}

#endif