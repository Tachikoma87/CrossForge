/*****************************************************************************\
*                                                                            *
* File(s): exampleSkeletalAnimation.hpp                                      *
*                                                                            *
* Content: Example scene that shows how to use skeletal animation.           *
*                                                                            *
*                                                                            *
*                                                                            *
* Author(s): Tom Uhlmann                                                     *
*                                                                            *
*                                                                            *
* The file(s) mentioned above are provided as is under the terms of the      *
* MIT License without any warranty or guaranty to work properly.             *
* For additional license, copyright and contact/support issues see the       *
* supplied documentation.                                                    *
*                                                                            *
\*****************************************************************************/

#ifndef __CFORGE_EXAMPLEREALTIMEMOTION_HPP__
#define __CFORGE_EXAMPLEREALTIMEMOTION_HPP__

#include <crossforge/Graphics/Actors/SkeletalActor.h>
#include <crossforge/Graphics/Actors/StickFigureActor.h>
#include <crossforge/Internet/UDPSocket.h>
#include <iostream>
#include <sstream>
#include <vector>

#include "ExampleSceneBase.hpp"

using namespace Eigen;
using namespace std;


namespace CForge
{
	class ExampleRealtimeMotion : public ExampleSceneBase
	{
	public:

		ExampleRealtimeMotion()
		{
			m_WindowTitle = "CrossForge Example - Real Time Motion";
		}

		~ExampleRealtimeMotion()
		{
			clear();
		}

		void init() override
		{
			// Set up the graphical interface
			initWindowAndRenderDevice();
			initCameraAndLights();

			m_RootSGN.init(nullptr);
			m_SG.init(&m_RootSGN);

			initGroundPlane(&m_RootSGN, 100.0f, 20.0f);
			initFPSLabel();
			initSkybox();

			T3DMesh<float> Mesh;
			SAssetIO::load("Assets/ExampleScenes/MuscleMan/MuscleMan.glb", &Mesh);
			setMeshShader(&Mesh, 0.7f, 0.04f);
			Mesh.computePerVertexNormals();
			m_BipedController.init(&Mesh);
			m_MuscleMan.init(&Mesh, &m_BipedController);
			m_MuscleManStick.init(&Mesh, &m_BipedController);

			SAssetIO::load("Assets/ExampleScenes/MuscleMan/TexturedUnitCube.gltf", &Mesh);
			setMeshShader(&Mesh, 0.1f, 0.04f);
			Mesh.computePerVertexNormals();

			// for (int index = 0; index < 21; ++index)
			// {
			// 	m_Cube[index].init(&Mesh);
			//}
			Mesh.clear();

			Quaternionf Rot;
			Rot = AngleAxisf(CForgeMath::degToRad(-90.0f), Vector3f::UnitX());

			m_MuscleManTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 0.0f, 0.0f), Rot, Vector3f(0.002f, 0.002f, 0.002f));
			m_MuscleManSGN.init(&m_MuscleManTransformSGN, &m_MuscleMan);
			m_MuscleManStickSGN.init(&m_MuscleManTransformSGN, &m_MuscleManStick);
			m_MuscleManSGN.enable(true, false);

			// for (int index = 0; index < 21; ++index)
			// {
			//	m_CubeTransformSGN[index].init(&m_RootSGN, Vector3f(0.0f, 0.0f + index * 0.5f, 0.0f));
			//	m_CubeSGN[index].init(&m_CubeTransformSGN[index], &m_Cube[index]);
			//	m_CubeSGN[index].scale(Vector3f(0.1f, 0.1f, 0.1f));
			// }

			std::string GLError = "";
			CForgeUtility::checkGLError(&GLError);

			if (!GLError.empty())
			{
				std::cout << "GLError occurred: " << GLError.c_str() << '\n';
			}

			// Establishe a connection to the UDP sender
			try
			{
				m_UDPServer.begin(UDPSocket::TYPE_SERVER, m_UDPPort);
			}
			catch (const CrossForgeException& exception)
			{
				std::cout << "UDPSocketError occurred: " << exception.msg() << '\n';
			}

			// Store the bones of the skeleton and prepare memory to store the global transformations
			m_NumberBones = m_BipedController.retrieveSkeleton().size();
			m_GlobalTransforms.resize(m_NumberBones);
		}

		void clear() override
		{
			m_UDPServer.end();
			ExampleSceneBase::clear();
		}

		void mainLoop() override
		{			
			// Read new data from the UDP stream
			if (m_UDPServer.recvData(m_Buffer, &m_DataSize, &m_Sender, &m_Port))
			{
				parseUDPStream();
				m_Time = 0;

				for (int index = 0; index < m_NumberBones; ++index)
				{
					m_BipedController.joints()[index]->LastLocalRotation = m_BipedController.joints()[index]->LocalRotation;
					m_BipedController.joints()[index]->LastLocalPosition = m_BipedController.joints()[index]->LocalPosition;
					m_BipedController.joints()[index]->LastLocalScale = m_BipedController.joints()[index]->LocalScale;
				}
			}

			if (m_Time >= 0)
			{
				// Compute the local transformations and apply this transformations to every bone
				calculateLocalTransformations(m_BipedController.root(), Matrix4f::Identity(), m_GlobalTransforms);
			}
			
			// Render the scene
			m_RenderWin.update();
			m_SG.update(60.0f / m_FPS);
			m_BipedController.update(60.0f / m_FPS);

			defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_1, true))
			{
				bool enabled = true;
				m_MuscleManSGN.enabled(nullptr, &enabled);

				if (enabled)
				{
					m_MuscleManSGN.enable(true, false);
					m_MuscleManStickSGN.enable(true, true);
				}
				else {
					m_MuscleManSGN.enable(true, true);
					m_MuscleManStickSGN.enable(true, false);
				}
			}

			m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			m_RenderDev.activeCamera(const_cast<VirtualCamera*>(m_Sun.camera()));
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_GEOMETRY);
			m_RenderDev.activeCamera(&m_Cam);
			m_SG.render(&m_RenderDev);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_LIGHTING);

			m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD, nullptr, false);
			m_SkyboxSG.render(&m_RenderDev);

			if (m_FPSLabelActive)
			{
				m_FPSLabel.render(&m_RenderDev);
			}

			m_RenderWin.swapBuffers();

			updateFPS();
			defaultKeyboardUpdate(m_RenderWin.keyboard());
		}


	protected:

		void calculateLocalTransformations(SkeletalAnimationController::Joint* pJoint, Eigen::Matrix4f ParentGlobalTransform, std::vector<Eigen::Matrix4f>& GlobalTransforms)
		{
			calculateLocalTransformationsRecursive(pJoint, ParentGlobalTransform, GlobalTransforms);

			// if (m_Time < m_UDPtimeDelta)
			// {
			// 	float s = m_Time / m_UDPtimeDelta;
			// 
			// 	for (int index = 0; index < m_NumberBones; ++index)
			// 	{
			// 		m_BipedController.joints()[index]->LocalRotation = m_BipedController.joints()[index]->LastLocalRotation.slerp(s, m_BipedController.joints()[index]->LocalRotation);
			// 		m_BipedController.joints()[index]->LocalPosition = (1.0f - s) * m_BipedController.joints()[index]->LastLocalPosition + s * m_BipedController.joints()[index]->LocalPosition;
			// 		m_BipedController.joints()[index]->LocalScale = (1.0f - s) * m_BipedController.joints()[index]->LastLocalScale + s * m_BipedController.joints()[index]->LocalScale;
			// 	}
			// }

			m_BipedController.transformSkeleton(m_BipedController.root(), Eigen::Matrix4f::Identity());

			for (int index = 0; index < m_NumberBones; ++index)
			{				
				m_BipedController.ubo()->skinningMatrix(index, m_BipedController.joints()[index]->SkinningMatrix);
			}

			m_Time += 1000.0f / m_FPS;
		}

		void calculateLocalTransformationsRecursive(SkeletalAnimationController::Joint* pJoint, Eigen::Matrix4f ParentTransform, std::vector<Eigen::Matrix4f>& GlobalTransforms)
		{
			if (nullptr == pJoint)
			{
				throw NullpointerExcept("pJoint");
			}

			Eigen::Affine3f Transformation(ParentTransform.inverse() * GlobalTransforms[pJoint->ID]);
			pJoint->LocalRotation = Transformation.rotation();
			pJoint->LocalPosition = Transformation.translation();
			pJoint->LocalScale = Vector3f::Ones();

			for (SkeletalAnimationController::Joint* child : pJoint->Children)
			{
				calculateLocalTransformationsRecursive(child, GlobalTransforms[pJoint->ID], GlobalTransforms);
			}
		}

		void parseUDPStream()
		{			
			m_Stream.clear();
			m_Stream.str(reinterpret_cast<const char*>(m_Buffer));
			m_JointIndex = 0;

			// Process the token including '['
			while (std::getline(m_Stream, m_Token, '['))
			{
				// Read the characters until ']' occures as a separate token
				if (std::getline(m_Stream, m_Token, ']'))
				{
					// Process tokens inside brackets
					m_BracketedStream.clear();
					m_BracketedStream.str(m_Token);

					for (m_Index = 0; m_BracketedStream >> m_BracketedToken; ++m_Index)
					{
						if (m_Index < 3)
						{
							m_Translation[m_Index] = std::stof(m_BracketedToken);
						}
						// else if (m_Index < 6)
						// {
						// 	m_EulerAngles[m_Index - 3] = std::stof(m_BracketedToken);
						// }

						if (m_Index < 9)
						{
							m_RotationMatrix(m_Index % 3, m_Index / 3) = std::stof(m_BracketedToken);
						}
					}

					// If the bracket does not contain exactly six elements, it does not contain the translation
					// and rotation information
					if (m_Index == 6)
					{
						// std::cout << m_Translation << std::endl;
						m_GlobalTransforms[m_JointIndex] = Matrix4f::Identity();
						m_GlobalTransforms[m_JointIndex].block<3, 1>(0, 3) = m_Translation;

						// m_CubeTransformSGN[m_JointIndex].translation(Vector3f(m_Translation[0] * 0.002f, m_Translation[2] * 0.002f, -m_Translation[1] * 0.002f));
					}
					else if (m_Index == 9)
					{
						// std::cout << m_RotationMatrix << std::endl;
						m_GlobalTransforms[m_JointIndex].block<3, 3>(0, 0) = m_RotationMatrix;
						m_JointIndex++;
					}
				}
			}
		}

		SGNTransformation m_RootSGN;
		SkeletalActor m_MuscleMan;
		StickFigureActor m_MuscleManStick;
		SkeletalAnimationController m_BipedController;
		SGNGeometry m_MuscleManSGN;
		SGNGeometry m_MuscleManStickSGN;
		SGNTransformation m_MuscleManTransformSGN;
		// StaticActor m_Cube[21];
		// SGNGeometry m_CubeSGN[21];
		// SGNTransformation m_CubeTransformSGN[21];

		UDPSocket m_UDPServer;
		int32_t m_UDPPort = 12345;
		int32_t m_UDPMsgCounter = 0;

		float m_Time = -1;
		float m_UDPfps = 60;
		float m_UDPtimeDelta = 1000.0f / m_UDPfps;

		std::string m_Sender;
		uint16_t m_Port;
		uint8_t m_Buffer[4096];
		uint32_t m_DataSize;

		std::istringstream m_Stream;
		std::istringstream m_BracketedStream;
		Vector3f m_Translation;
		// Vector3f m_EulerAngles;
		Matrix3f m_RotationMatrix;
		Quaternionf m_Rotation;
		std::string m_Token;
		std::string m_BracketedToken;
		int m_JointIndex;
		int m_Index;

		std::vector<Eigen::Matrix4f> m_GlobalTransforms;
		int m_NumberBones;
	};
}

#endif