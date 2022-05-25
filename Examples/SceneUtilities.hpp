/*****************************************************************************\
*                                                                           *
* File(s): SceneUtilities.hpp                                            *
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
#ifndef __CFORGE_SCENEUTILITIES_HPP__
#define __CFORGE_SCENEUTILITIES_HPP__

#include "../CForge/Graphics/GraphicsUtility.h"

#include "../CForge/Input/Keyboard.h"
#include "../CForge/Input/Mouse.h"
#include "../CForge/Graphics/VirtualCamera.h"

#include "../CForge/AssetIO/T3DMesh.hpp"


namespace CForge {

	class SceneUtilities {
	public:
		static void setMeshShader(T3DMesh<float>* pM, float Roughness, float Metallic) {
			for (uint32_t i = 0; i < pM->materialCount(); ++i) {
				T3DMesh<float>::Material* pMat = pM->getMaterial(i);
				/*pMat->VertexShaderSources.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderSources.push_back("Shader/BasicGeometryPass.frag");*/

				pMat->VertexShaderGeometryPass.push_back("Shader/BasicGeometryPass.vert");
				pMat->FragmentShaderGeometryPass.push_back("Shader/BasicGeometryPass.frag");

				pMat->VertexShaderShadowPass.push_back("Shader/ShadowPassShader.vert");
				pMat->FragmentShaderShadowPass.push_back("Shader/ShadowPassShader.frag");

				pMat->Metallic = Metallic;
				pMat->Roughness = Roughness;
			}//for[materials]
		}//setMeshShader

		static void defaultCameraUpdate(VirtualCamera* pCamera, Keyboard* pKeyboard, Mouse* pMouse, const float MovementSpeed = 0.4f, const float RotationSpeed = 1.0f, const float SpeedScale = 4.0f) {
			if (nullptr == pCamera) throw NullpointerExcept("pCamera");
			if (nullptr == pKeyboard) throw NullpointerExcept("pKeyboard");
			if (nullptr == pMouse) throw NullpointerExcept("pMouse");

			float S = 1.0f;
			if (pKeyboard->keyPressed(Keyboard::KEY_LEFT_SHIFT)) S = SpeedScale;

			if (pKeyboard->keyPressed(Keyboard::KEY_W)) pCamera->forward(S * MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_S)) pCamera->forward(S * -MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_A)) pCamera->right(-S * MovementSpeed);
			if (pKeyboard->keyPressed(Keyboard::KEY_D)) pCamera->right(S * MovementSpeed);

			if (pMouse->buttonState(Mouse::BTN_RIGHT)) {
				const Eigen::Vector2f MouseDelta = pMouse->movement();
				pCamera->rotY(GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.x()));
				pCamera->pitch(GraphicsUtility::degToRad(-0.1f * RotationSpeed * MouseDelta.y()));
				pMouse->movement(Eigen::Vector2f::Zero());
			}
		}//defaultCameraUpdate

		static void takeScreenshot(std::string Filepath) {
			T2DImage<uint8_t> ColorBuffer;
			GraphicsUtility::retrieveFrameBuffer(&ColorBuffer);
			SAssetIO::store(Filepath, &ColorBuffer);
		}//takeScreen

	};// SceneUtilities

}//name space

#endif 