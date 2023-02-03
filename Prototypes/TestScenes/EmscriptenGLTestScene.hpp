/*****************************************************************************\
*                                                                           *
* File(s): EmscriptenGLTestScene.hpp                                            *
*                                                                           *
* Content: Example scene that shows minimum setup with an OpenGL capable   *
*          window, lighting setup, and a single moving object.              *
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
#ifndef __CFORGE_EMSCRIPTENGLTESTSCENE_HPP__
#define __CFORGE_EMSCRIPTENGLTESTSCENE_HPP__


#include "../../Examples/ExampleSceneBase.hpp"
#include <CForge/MeshProcessing/PrimitiveShapeFactory.h>

using namespace Eigen;
using namespace std;

namespace CForge {

	class EmscriptenGLTestScene : public ExampleSceneBase {
	public:
		EmscriptenGLTestScene(void) {
			m_WindowTitle = "CrossForge Example - Emscripten GL Test-Scene";
			m_WinWidth = 1280;
			m_WinHeight = 720;
		}//Constructor

		~EmscriptenGLTestScene(void) {
			clear();
		}//Destructor

		void init() override {

			/*initWindowAndRenderDevice(false);
			initCameraAndLights();*/

			m_RenderWin.init(Vector2i(100, 100), Vector2i(m_WinWidth, m_WinHeight), m_WindowTitle);

			m_pShaderMan = SShaderManager::instance();

#ifndef __EMSCRIPTEN__
			gladLoadGL();
#endif

			// load skydome and a textured cube
			T3DMesh<float> M;

			//PrimitiveShapeFactory::uvSphere(&M, Vector3f(2, 2, 2), 5, 5);		
			//setMeshShader(&M, 0.2f, 0.04f);
			//CForgeUtility::defaultMaterial(M.getMaterial(0), CForgeUtility::METAL_GOLD);

			////M.changeUVTiling(Vector3f(5.0f, 5.0f, 1.0f));
			//std::string ErrorMsg;
			//if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
			//	SLogger::log("Not handled OpenGL error occurred during initialization of Sphere: " + ErrorMsg, "Sphere", SLogger::LOGTYPE_ERROR);
			//}

			//M.computePerVertexNormals();
			////M.computePerVertexTangents();
			//m_Duck.init(&M);
			//M.clear();

			//BoundingVolume BV;
			//m_Duck.boundingVolume(BV);

			//// build scene graph
			//m_RootSGN.init(nullptr);
			//m_SG.init(&m_RootSGN);

			//// add skydome
			//m_SkydomeSGN.init(&m_RootSGN, &m_Skydome);
			//m_SkydomeSGN.scale(Vector3f(50.0f, 50.0f, 50.0f));

			//// add cube
			//m_DuckTransformSGN.init(&m_RootSGN, Vector3f(0.0f, 2.5f, 0.0f));
			//m_DuckSGN.init(&m_DuckTransformSGN, &m_Duck);
			////m_DuckSGN.scale(Vector3f(0.02f, 0.02f, 0.02f));

			//// rotate about the y-axis at 45 degree every second
			//Quaternionf R = Quaternionf::Identity();
			//R = AngleAxisf(CForgeMath::degToRad(45.0f / 60.0f), Vector3f::UnitY());
			//m_DuckTransformSGN.rotationDelta(R);

			// stuff for performance monitoring
			uint64_t LastFPSPrint = CForgeUtility::timestamp();
			int32_t FPSCount = 0;

			buildRenderStuff();

			std::string ErrorMsg;
			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("Not handled OpenGL error occurred during initialization of scene: " + ErrorMsg, "Scne", SLogger::LOGTYPE_ERROR);
			}

			m_BGColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		}//initialize

		void clear(void) override {
			m_RenderWin.stopListening(this);
			if (nullptr != m_pShaderMan) m_pShaderMan->release();
			m_pShaderMan = nullptr;
		}//clear

		void mainLoop(void)override {
			m_RenderWin.update();
			//m_SG.update(60.0f / m_FPS);

			//defaultCameraUpdate(&m_Cam, m_RenderWin.keyboard(), m_RenderWin.mouse());

			float Val = (CForgeUtility::timestamp() % 10000) / 1000.0f;

			m_BGColor.x() = (std::sin(Val) + 1.0f)/2.0f;
			m_BGColor.y() = (std::cos(Val) + 1.0f) / 2.0f;
			glClearColor(m_BGColor.x(), m_BGColor.y(), m_BGColor.z(), 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//m_RenderDev.activePass(RenderDevice::RENDERPASS_SHADOW, &m_Sun);
			//m_SG.render(&m_RenderDev);
			/*m_RenderDev.activePass(RenderDevice::RENDERPASS_FORWARD);
			m_SG.render(&m_RenderDev);
			*/

			
			
			static Vector3f Move = Vector3f::Zero();
			
			Val = CForgeUtility::timestamp()% 100000;
			Move.x() = std::sin(Val/5000.0f)/2.0f;
			
			//m_RenderDev.modelUBO()->modelMatrix(CForgeMath::translationMatrix(Move));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			m_pSimpleShader->bind();

			glBindVertexArray(m_VertexArray);
			uint32_t OffsetLoc = m_pSimpleShader->uniformLocation("XOffset");
			glUniform1f(OffsetLoc, std::abs(Move.x()));
			//m_RenderDev.modelUBO()->bind(0);

			uint32_t PosLoc = m_pSimpleShader->uniformBlockIndex("ModelData2");
			//printf("PosLoc: %d\n", PosLoc);
			m_pSimpleShader->uniformBlockBinding(PosLoc, PosLoc);

			glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, PosLoc, PosLoc);
			glBindBufferRange(GL_UNIFORM_BUFFER, PosLoc, m_UniformBuffer, 0, sizeof(float) * 16);
			//uint32_t BindingPoint = m_pSimpleShader->uboBindingPoint(GLShader::DEFAULTUBO_MODELDATA);
			//glUniformBLockBinding(m_SimpleShader.)

			//m_RenderDev.modelUBO()->modelMatrix(CForgeMath::translationMatrix(Vector3f(0.25f, 0.25f, 0.0f)));
			//printf("Binding Point: %d\n", BindingPoint);

			//m_pSimpleShader->uniformBlockBinding(BindingPoint, BindingPoint);
			//m_RenderDev.modelUBO()->bind(BindingPoint);


			
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);

			m_RenderWin.swapBuffers();
			updateFPS();

		
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_UP, true)) {
				float Metal = m_Duck.material(0)->metallic();
				m_Duck.material(0)->metallic(Metal + 0.05f);
				printf("Metallic now: %.2f\n", m_Duck.material(0)->metallic());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_DOWN, true)) {
				float Metal = m_Duck.material(0)->metallic();
				m_Duck.material(0)->metallic(Metal - 0.05f);
				printf("Metallic now: %.2f\n", m_Duck.material(0)->metallic());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_LEFT, true)) {
				float Roughness = m_Duck.material(0)->roughness();
				m_Duck.material(0)->roughness(Roughness - 0.05f);
				printf("Roughness now: %.2f\n", m_Duck.material(0)->roughness());
			}
			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_RIGHT, true)) {
				float Roughness = m_Duck.material(0)->roughness();
				m_Duck.material(0)->roughness(Roughness + 0.05f);
				printf("Roughness now: %.2f\n", m_Duck.material(0)->roughness());
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_M, true)) {
				m_CurrentMaterial++;
				if (m_CurrentMaterial > uint32_t(CForgeUtility::DEFAULT_MATERIAL_COUNT)) m_CurrentMaterial = 0;
				CForgeUtility::defaultMaterial(m_Duck.material(0), (CForgeUtility::DefaultMaterial)m_CurrentMaterial);
			}

			if (m_RenderWin.keyboard()->keyPressed(Keyboard::KEY_F2, true)) {
				SGNGeometry::Visualization Vis = m_DuckSGN.visualization();
				m_DuckSGN.visualization((Vis == SGNGeometry::VISUALIZATION_WIREFRAME) ? SGNGeometry::VISUALIZATION_FILL : SGNGeometry::VISUALIZATION_WIREFRAME);

			}

			defaultKeyboardUpdate(m_RenderWin.keyboard());

			std::string ErrorMsg;
			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				//SLogger::log("OpenGL Error" + ErrorMsg, "PrimitiveFactoryTestScene", SLogger::LOGTYPE_ERROR);
			}
		}//mainLoop

	protected:

		void buildRenderStuff(void) {
			// shader stuff
#ifdef __EMSCRIPTEN__
			const char* vert = 
R"(#version 300 es \r\n			
in vec4 Position; \r\n
uniform ModelData{ \r\n
	mat4 ModelMatrix; \r\n
	mat4x3 NormalMatrix; \r\n
}Model;\r\n
void main(void){ \r\n
	gl_Position = Position; \r\n
}\r\n
			)";
			const char* frag = R"(#version 300 es
				precision mediump float;
				out vec4 FragColor;

				void main(void){
					FragColor = vec4(1.0, 0.0, 0.0, 1.0);
				}
			)";
#else
			const char* vert = R"(#version 330 core
				in vec4 Position;

				layout(std140) uniform ModelData{
					mat4 ModelMatrix;
					mat4x3 NormalMatrix;
				}Model;

				void main(void){
					gl_Position = Model.ModelMatrix * Position;
				}
			)";
			const char* frag = R"(#version 330 core
				out vec4 FragColor;
				void main(void){
					FragColor = vec4(1.0, 0.0, 0.0, 1.0);
				}
			)";
#endif
			glGenVertexArrays(1, &m_VertexArray);
			glBindVertexArray(m_VertexArray);


			std::string ErrorLog;

			std::string VersionTag = "330 core";
			std::string PrecisionTag = "highp";

#ifdef __EMSCRIPTEN__
			VersionTag = "300 es";
			PrecisionTag = "mediump";
#endif
			std::vector<ShaderCode*> VSShaders;
			std::vector<ShaderCode*> FSShaders;
			VSShaders.push_back(m_pShaderMan->createShaderCode("Shader/EMSTest.vert", VersionTag, 0, PrecisionTag));
			FSShaders.push_back(m_pShaderMan->createShaderCode("Shader/EMSTest.frag", VersionTag, 0, PrecisionTag));
			m_pSimpleShader = m_pShaderMan->buildShader(&VSShaders, &FSShaders, &ErrorLog);

			/*m_SimpleShader.init();
			m_SimpleShader.addVertexShader(vert);
			m_SimpleShader.addFragmentShader(frag);*/
			std::string ErrorMsg;
			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("Not handled OpenGL error occurred before building shader: " + ErrorMsg, "EmscriptenGLTestScene", SLogger::LOGTYPE_ERROR);
			}

			//m_SimpleShader.build(&ErrorLog);
			if (!ErrorLog.empty()) SLogger::log("Compiling shader failed! " + ErrorLog, "EmscriptenGLTestScene");
			m_PositionAttribute = m_pSimpleShader->attribArrayIndex(GLShader::ATTRIB_POSITION);
			//printf("Shader: %s\n", vert);

			float vertexData[24] = {
			-0.5, -0.5, 0.0, 1.0 ,  0.0, 0.0,
			-0.5,  0.5, 0.0, 1.0 ,  0.0, 1.0,
			0.5,  0.5, 0.0, 1.0 ,  1.0, 1.0,
			0.5, -0.5, 0.0, 1.0 ,  1.0, 0.0,
			};


			glGenBuffers(1, &m_VertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(float), vertexData, GL_STATIC_DRAW);
			glEnableVertexAttribArray(m_PositionAttribute);
			int VertexSize = sizeof(float) * 4;
			glVertexAttribPointer(m_PositionAttribute, 4, GL_FLOAT, GL_FALSE, VertexSize, (const GLvoid*)0);



			
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			
			Matrix4f M = Matrix4f::Identity();
			glGenBuffers(1, &m_UniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 16, M.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			//glBindBuffer(GL_ELEMENT_ARRAY, 0);
			

			if (GL_NO_ERROR != CForgeUtility::checkGLError(&ErrorMsg)) {
				SLogger::log("OpenGL Error during initiaization of render stuff " + ErrorMsg, "EmscriptenGLTestScene", SLogger::LOGTYPE_ERROR);
			}
			/*uint32_t PosLoc = m_pSimpleShader->uniformBlockIndex("ModelData2");
			printf("PosLoc: %d\n", PosLoc);
			m_pSimpleShader->uniformBlockBinding(PosLoc, PosLoc);*/

		}//buildRenderStuff




		// Scene Graph
		SGNTransformation m_RootSGN;
		SGNGeometry m_SkydomeSGN;
		SGNGeometry m_DuckSGN;
		SGNTransformation m_DuckTransformSGN;

		StaticActor m_Skydome;
		StaticActor m_Duck;

		uint32_t m_CurrentMaterial = 0;
		Vector4f m_BGColor;

		GLShader *m_pSimpleShader;
		uint32_t m_VertexBuffer;
		uint32_t m_ElementBuffer;

		uint32_t m_PositionAttribute;
		uint32_t m_VertexArray;
		uint32_t m_UniformBuffer;
	};//ExampleMinimumGraphicsSetup

}//name space

#endif