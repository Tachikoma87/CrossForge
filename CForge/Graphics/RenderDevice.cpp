#include <glad/glad.h>
#include "../Core/SLogger.h"
#include "../Core/CoreUtility.hpp"
#include "Shader/SShaderManager.h"
#include "GraphicsUtility.h"
#include "RenderDevice.h"

using namespace Eigen;
using namespace std;

namespace CForge {

	RenderDevice::RenderDeviceConfig::RenderDeviceConfig(void) {

	}//constructor

	RenderDevice::RenderDeviceConfig::~RenderDeviceConfig(void) {

	}//Destructor

	void RenderDevice::RenderDeviceConfig::init(void) {
		DirectionalLightsCount = 1;
		PointLightsCount = 4;
		SpotLightsCount = 1;

		GBufferWidth = 1280;
		GBufferHeight = 720;

		pAttachedWindow = nullptr;
		MatchGBufferAndWindow = true;
		PhysicallyBasedShading = true;

		UseGBuffer = true;
		ExecuteLightingPass = true;
	}

	RenderDevice::RenderDevice(void): CForgeObject("RenderDevice") {
		m_pActiveCamera = nullptr;
		m_pActiveMaterial = nullptr;
		m_pActiveShader = nullptr;
	}//Constructor

	RenderDevice::~RenderDevice(void) {
		clear();
	}//Destructor

	void RenderDevice::init(RenderDeviceConfig *pConfig) {
		clear();

		if (nullptr == pConfig) m_Config.init();
		else m_Config = (*pConfig);

		m_CameraUBO.init();
		m_ModelUBO.init();
		m_MaterialUBO.init();
		m_LightsUBO.init(m_Config.DirectionalLightsCount, m_Config.PointLightsCount, m_Config.SpotLightsCount);
		
		m_InstancesUBO.init();

		// use GBuffer?
		if (m_Config.UseGBuffer) {
			if (m_Config.GBufferHeight == 0 || m_Config.GBufferWidth == 0) {
				SLogger::log("Invalid GBuffer Height and Width specified. Defaulting to 1280x720");
				m_Config.GBufferHeight = 720;
				m_Config.GBufferWidth = 1280;
			}
			m_GBuffer.init(m_Config.GBufferWidth, m_Config.GBufferHeight);

			if (m_Config.ExecuteLightingPass) {
				m_ScreenQuad.init(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
				SShaderManager* pSMan = SShaderManager::instance();
				string ErrorLog;

				std::vector<ShaderCode*> VSSources;
				std::vector<ShaderCode*> FSSources;
				ShaderCode* pSC = nullptr;

				if (m_Config.PhysicallyBasedShading) {
					pSC = pSMan->createShaderCode("Shader/DRLightingPassPBS.vert", "330 core", 0, "highp", "highp");
					VSSources.push_back(pSC);
					pSC = pSMan->createShaderCode("Shader/DRLightingPassPBS.frag", "330 core", ShaderCode::CONF_LIGHTING | ShaderCode::CONF_POSTPROCESSING, "highp", "highp");
					FSSources.push_back(pSC);
					m_pDeferredLightingPassShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);
				}
				else {
					pSC = pSMan->createShaderCode("Shader/DRLightingPassBlinnPhong.vert", "330 core", 0, "highp", "highp");
					VSSources.push_back(pSC);
					pSC = pSMan->createShaderCode("Shader/DRLightingPassBlinnPhong.frag", "330 core", ShaderCode::CONF_LIGHTING | ShaderCode::CONF_POSTPROCESSING, "highp", "highp");
					FSSources.push_back(pSC);
					m_pDeferredLightingPassShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);
				}
				
				if (nullptr == m_pDeferredLightingPassShader || !ErrorLog.empty()) {
					SLogger::log(ErrorLog);
					throw CForgeExcept("Building deferred lighting pass shader failed. See log for details.");
				}

				VSSources.clear();
				FSSources.clear();
				pSC = pSMan->createShaderCode("Shader/ShadowPassShader.vert", "330 core", ShaderCode::CONF_LIGHTING, "highp", "highp");
				VSSources.push_back(pSC);
				pSC = pSMan->createShaderCode("Shader/ShadowPassShader.frag", "330 core", 0, "highp", "highp");
				FSSources.push_back(pSC);
				m_pShadowPassShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);
				if (nullptr == m_pShadowPassShader || !ErrorLog.empty()) {
					SLogger::log(ErrorLog);
					throw CForgeExcept("Building shadow pass shader failed. See log for details.");
				}

				pSMan->release();
			}//if[lighting pass]
		}//if[GBuffer]

	}//initialize

	void RenderDevice::clear(void) {
		m_CameraUBO.clear();
	}//clear


	void RenderDevice::requestRendering(IRenderableActor* pActor, Eigen::Quaternionf Rotation, Eigen::Vector3f Translation, Eigen::Vector3f Scale) {
		if (nullptr == pActor) throw NullpointerExcept("pActor");	

		////// create model matrix and update buffer
		const Matrix4f R = GraphicsUtility::rotationMatrix(Rotation);
		const Matrix4f T = GraphicsUtility::translationMatrix(Translation);
		const Matrix4f S = GraphicsUtility::scaleMatrix(Scale);
		const Matrix4f ModelMat = T * R * S;
		
		requestRendering(pActor, ModelMat);
	}//requestRendering
	
	void RenderDevice::requestRendering(IRenderableActor* pActor, Eigen::Matrix4f ModelMat) {
		if (nullptr == pActor) throw NullpointerExcept("pActor");
		
		m_ModelUBO.modelMatrix(ModelMat);
		if (m_ActiveRenderPass == RENDERPASS_LOD) {
			// automatic instanced rendering
			// TODO make aabb mesh global instead of per actor?
			if (pActor->isInstanced()) {
				pActor->testAABBvis(this, ModelMat);
			}
			else {
				if (pActor->className().compare("LODActor") != 0) {
					m_LODSGActors.push_back(pActor);
					m_LODSGTransformations.push_back(ModelMat);
				}
				else {
					pActor->testAABBvis(this, ModelMat);
				}
			}
		}
		else
		{
			// render the object with current settings
			pActor->render(this);
		}
	}//requestRendering

	void RenderDevice::activeShader(GLShader* pShader) {
		if (pShader != m_pActiveShader) {
			m_pActiveShader = pShader;
			m_pActiveShader->bind();
			
			uint32_t BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_CAMERADATA);
			if (GL_INVALID_INDEX != BindingPoint) {
				m_CameraUBO.bind(BindingPoint);
			}

			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_MODELDATA);
			if (GL_INVALID_INDEX != BindingPoint) {
				m_ModelUBO.bind(BindingPoint);
			}
			
			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_INSTANCE);
			if (GL_INVALID_INDEX != BindingPoint) {
				m_InstancesUBO.bind(BindingPoint);
			}
			else {
				//std::cout << "InstanceUBO: Invalid binding Point.\n";
			}
			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_MATERIALDATA);
			if (GL_INVALID_INDEX != BindingPoint) m_MaterialUBO.bind(BindingPoint);

			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_DIRECTIONALLIGHTSDATA);
			if (GL_INVALID_INDEX != BindingPoint) m_LightsUBO.bind(BindingPoint, ILight::LIGHT_DIRECTIONAL);
			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_POINTLIGHTSDATA);
			if (GL_INVALID_INDEX != BindingPoint) m_LightsUBO.bind(BindingPoint, ILight::LIGHT_POINT);
			BindingPoint = m_pActiveShader->uboBindingPoint(GLShader::DEFAULTUBO_SPOTLIGHTSDATA);
			if (GL_INVALID_INDEX != BindingPoint) m_LightsUBO.bind(BindingPoint, ILight::LIGHT_SPOT);

			// set shadow maps
			for (auto i : m_ShadowCastingLights) {
				if (i->pLight != nullptr && i->DefaultTexture != GLShader::DEFAULTTEX_UNKNOWN) {
					i->pLight->bindShadowTexture(m_pActiveShader, i->DefaultTexture);
				}
			}//for[shadow casting lights]

			// update material UBO
			updateMaterial();
		}//if[different shader]
		
	}//activeShader

	void RenderDevice::activeMaterial(RenderMaterial* pMaterial) {
		if (pMaterial != m_pActiveMaterial) {
			m_pActiveMaterial = pMaterial;
			updateMaterial();
		}
	}//activeMaterial

	void RenderDevice::activeCamera(VirtualCamera* pCamera) {
		if (nullptr != m_pActiveCamera) {
			m_pActiveCamera->stopListening(this);
		}
		if (pCamera != m_pActiveCamera) {
			m_pActiveCamera = pCamera;
			m_CameraUBO.viewMatrix(m_pActiveCamera->cameraMatrix());
			m_CameraUBO.projectionMatrix(m_pActiveCamera->projectionMatrix());
			m_CameraUBO.position(m_pActiveCamera->position());
			m_pActiveCamera->startListening(this);
		}
	}//activeCamera


	GLShader* RenderDevice::RenderDevice::activeShader(void)const {
		return m_pActiveShader;
	}//activeShader

	RenderMaterial* RenderDevice::activeMaterial(void)const {
		return m_pActiveMaterial;
	}//activeMaterial

	VirtualCamera* RenderDevice::activeCamera(void)const {
		return m_pActiveCamera;
	}//activeCamera

	UBOCameraData* RenderDevice::cameraUBO(void) {
		return &m_CameraUBO;
	}//cameraUBO

	UBOModelData* RenderDevice::modelUBO(void) {
		return &m_ModelUBO;
	}//modelUBO

	UBOLightData* RenderDevice::lightsUBO(void) {
		return &m_LightsUBO;
	}//lightsUBO


	void RenderDevice::listen(const VirtualCameraMsg Msg) {
		switch (Msg.Code) {
		case VirtualCameraMsg::POSITION_CHANGED: {
			m_CameraUBO.position(m_pActiveCamera->position());
			m_CameraUBO.viewMatrix(m_pActiveCamera->cameraMatrix());
		}break;
		case VirtualCameraMsg::ROTATION_CHANGED: {
			m_CameraUBO.viewMatrix(m_pActiveCamera->cameraMatrix());
		}break;
		case VirtualCameraMsg::PROJECTION_CHANGED: {
			m_CameraUBO.projectionMatrix(m_pActiveCamera->projectionMatrix());
		}break;
		}
	}//listen

	void RenderDevice::updateMaterial(void) {
		if (nullptr != m_pActiveMaterial && nullptr != m_pActiveShader) {
			if (nullptr != m_pActiveMaterial->albedoMap()) m_pActiveShader->bindTexture(GLShader::DEFAULTTEX_ALBEDO, m_pActiveMaterial->albedoMap());
			if (nullptr != m_pActiveMaterial->normalMap()) m_pActiveShader->bindTexture(GLShader::DEFAULTTEX_NORMAL, m_pActiveMaterial->normalMap());
			if (nullptr != m_pActiveMaterial->depthMap()) m_pActiveShader->bindTexture(GLShader::DEFAULTTEX_DEPTH, m_pActiveMaterial->depthMap());

			m_MaterialUBO.color(m_pActiveMaterial->color());
			m_MaterialUBO.metallic(m_pActiveMaterial->metallic());
			m_MaterialUBO.roughness(m_pActiveMaterial->roughness());
			m_MaterialUBO.ambientOcclusion(m_pActiveMaterial->ambientOcclusion());

		}
	}//updateMaterial

	void RenderDevice::activePass(RenderPass Pass, ILight *pActiveLight) {
		
		bool previousPassWasLOD = false;
		// enable drawing after LOD pass
		if (m_ActiveRenderPass == RENDERPASS_LOD /* && Pass != RENDERPASS_LOD*/) {
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			previousPassWasLOD = true;
		}
		m_ActiveRenderPass = Pass;

		// change state?
		switch (m_ActiveRenderPass) {
		case RENDERPASS_SHADOW: {
			activeShader(m_pShadowPassShader);

			if (pActiveLight == nullptr) break;

			ActiveLight* pAL = nullptr;
			for (auto i : m_ShadowCastingLights) {
				if (i->pLight == pActiveLight) {
					pAL = i;
					break;
				}
			}//for[shadow casting lights]

			if (nullptr != pAL) {
				pAL->pLight->bindShadowFBO();
				glViewport(0, 0, pAL->pLight->shadowMapSize().x(), pAL->pLight->shadowMapSize().y());
				glClear(GL_DEPTH_BUFFER_BIT);

				uint32_t Loc = m_pActiveShader->uniformLocation("ActiveLightID");
				glUniform1ui(Loc, pAL->UBOIndex);
				glCullFace(GL_FRONT); // cull front face to solve peter-panning shadow artifact
			}
		}break;
		case RENDERPASS_GEOMETRY: {
			// bind geometry buffer
			if (m_Config.UseGBuffer) {
				if (!previousPassWasLOD)
					m_GBuffer.bind();
				glViewport(0, 0, m_GBuffer.width(), m_GBuffer.height());
				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_BACK);
			}
		}break;
		case RENDERPASS_LIGHTING: {
			if (m_Config.UseGBuffer) {
				m_GBuffer.unbind();
			}

			if (m_Config.ExecuteLightingPass) {
				glViewport(0, 0, m_Config.pAttachedWindow->width(), m_Config.pAttachedWindow->height());
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_BACK);

				activeShader(m_pDeferredLightingPassShader);
				uint32_t LocPos = m_pDeferredLightingPassShader->uniformLocation(GLShader::DEFAULTTEX_DEPTH);
				uint32_t LocNormal = m_pDeferredLightingPassShader->uniformLocation(GLShader::DEFAULTTEX_NORMAL);
				uint32_t LocAlbedo = m_pDeferredLightingPassShader->uniformLocation(GLShader::DEFAULTTEX_ALBEDO);

				if (LocPos != GL_INVALID_INDEX) {
					m_GBuffer.bindTexture(GBuffer::COMP_POSITION, LocPos);
					glUniform1i(LocPos, LocPos);
				}
				if (LocNormal != GL_INVALID_INDEX) {
					m_GBuffer.bindTexture(GBuffer::COMP_NORMAL, LocNormal);
					glUniform1i(LocNormal, LocNormal);
				}
				if (LocAlbedo != GL_INVALID_INDEX) {
					m_GBuffer.bindTexture(GBuffer::COMP_ALBEDO, LocAlbedo);
					glUniform1i(LocAlbedo, LocAlbedo);
				}
				requestRendering(&m_ScreenQuad, Quaternionf::Identity(), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f));
			}
		}break;
		case RENDERPASS_FORWARD: {
			if (m_Config.UseGBuffer) {
				// blit depth buffer
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				m_GBuffer.blitDepthBuffer(m_Config.pAttachedWindow->width(), m_Config.pAttachedWindow->height());
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, m_Config.pAttachedWindow->width(), m_Config.pAttachedWindow->height());
			}
		}break;
		case RENDERPASS_LOD: {
			if (m_Config.UseGBuffer) {
				m_GBuffer.bind();
				glViewport(0, 0, m_GBuffer.width(), m_GBuffer.height());
				glCullFace(GL_BACK);
			}
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
		}
		default: break;
		}

	}//activePass

	void RenderDevice::addLight(ILight* pLight) {
		if (nullptr == pLight) throw NullpointerExcept("pLight");

		switch (pLight->type()) {
		case ILight::LIGHT_DIRECTIONAL: addLight(pLight, &m_ActiveDirLights); break;
		case ILight::LIGHT_POINT: addLight(pLight, &m_ActivePointLights); break;
		case ILight::LIGHT_SPOT: addLight(pLight, &m_ActiveSpotLights); break;
		default: throw CForgeExcept("Invalid light type encountered!"); break;
		}

	}//addLight

	void RenderDevice::addLight(ILight *pLight, std::vector<ActiveLight*>* pLights) {
		// do we already know this light?
		for (auto i : (*pLights)) {
			if (i->pLight == pLight) return; // we already have this light
		}

		ActiveLight* pAL = new ActiveLight();
		pAL->UBOIndex = pLights->size();
		pAL->DefaultTexture = GLShader::DEFAULTTEX_UNKNOWN;
		pAL->pLight = pLight;
		pAL->ShadowIndex = -1;
		pLights->push_back(pAL);
		m_LightsUBO.updateLight(pLight, pAL->UBOIndex);
		m_LightsUBO.shadowID(-1, pAL->pLight->type(), pAL->UBOIndex);

		if (pLight->castsShadows()) {
			// find free index
			for (uint32_t i = 0; i < m_ShadowCastingLights.size(); ++i) {
				if (m_ShadowCastingLights[i]->pLight == nullptr) {
					pAL->ShadowIndex = i;
					break;
				}
			}//for[shadow casting lights]
			if (pAL->ShadowIndex == -1) {
				pAL->ShadowIndex = m_ShadowCastingLights.size();
				m_ShadowCastingLights.push_back(pAL);
			}
			else {
				m_ShadowCastingLights[pAL->ShadowIndex] = pAL;
			}

			// initialize shadow casting light
			switch (pAL->ShadowIndex) {
				case 0: pAL->DefaultTexture = GLShader::DEFAULTTEX_SHADOW0; break;
				case 1: pAL->DefaultTexture = GLShader::DEFAULTTEX_SHADOW1; break;
				case 2: pAL->DefaultTexture = GLShader::DEFAULTTEX_SHADOW2; break;
				case 3: pAL->DefaultTexture = GLShader::DEFAULTTEX_SHADOW3; break;
				default: pAL->DefaultTexture = GLShader::DEFAULTTEX_UNKNOWN; break;
			}//switch[default texture]

			m_LightsUBO.shadowID(pAL->ShadowIndex, pLight->type(), pAL->UBOIndex);
			Matrix4f LightSpaceMatrix = pAL->pLight->projectionMatrix() * pAL->pLight->viewMatrix();
			m_LightsUBO.lightSpaceMatrix(LightSpaceMatrix, pAL->pLight->type(), pAL->UBOIndex);
					
		}//if[cast shadows]
		
	}//addLight

	void RenderDevice::removeLight(ILight* pLight) {
		if (nullptr == pLight) throw NullpointerExcept("pLight");
		throw CForgeExcept("Not implemented yet!");	
	}//removeLight

	uint32_t RenderDevice::activeLightsCount(ILight::LightType Type)const {
		uint32_t Rval = 0;
		switch (Type) {
		case ILight::LIGHT_DIRECTIONAL: Rval = m_ActiveDirLights.size(); break;
		case ILight::LIGHT_POINT: Rval = m_ActiveDirLights.size(); break;
		case ILight::LIGHT_SPOT: Rval = m_ActiveDirLights.size(); break;
		default: throw CForgeExcept("Unknown light type specified!"); break;
		}
		return Rval;
	}//activeLightsCount

	GBuffer* RenderDevice::gBuffer(void) {
		return &m_GBuffer;
	}//gBuffer

	RenderDevice::RenderPass RenderDevice::activePass(void)const {
		return m_ActiveRenderPass;
	}//activePass
	
	GLShader* RenderDevice::shadowPassShader(void) {
		return m_pShadowPassShader;
	}//shadowPassShader
	
	void RenderDevice::renderLODSG()
	{
		fetchQueryResults();
		AssembleLODSG();
		
		for (uint32_t i = 0; i < m_LODSGActors.size(); i++) {
			requestRendering(m_LODSGActors[i], m_LODSGTransformations[i]);
			m_LODSGActors[i]->setLODSG(false);
		}
		m_LODSGActors.clear();
		m_LODSGTransformations.clear();
	}
	void RenderDevice::clearBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderDevice::setModelMatrix(Eigen::Matrix4f matrix) {
		m_ModelUBO.modelMatrix(matrix);
	}
	
	UBOInstancedData* RenderDevice::getInstancedUBO() {
		return &m_InstancesUBO;
	}

	void RenderDevice::fetchQueryResults() {
		
		for (uint32_t i = 0; i < LODQueryContainers.size(); i++) {
			LODQueryContainer* queryContainer = &LODQueryContainers[i];
			GLuint pixelCount;
			GLint queryState;
			GLuint queryID = queryContainer->queryID;
			if (!glIsQuery(queryID) || queryContainer->pixelCount != 0)
				continue;
			do {
				glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &queryState);
			} while (!queryState);
			glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &pixelCount);
			glDeleteQueries(1, &queryID);
			
			queryContainer->pixelCount = pixelCount;
			queryContainer->queryID = 0;
		}
	}

	void RenderDevice::LODQueryContainerPushBack(GLuint queryID, IRenderableActor* pActor, Eigen::Matrix4f transform) {
		LODQueryContainer container;
		container.queryID = queryID;
		container.pActor = pActor;
		container.transform = transform;
		LODQueryContainers.push_back(container);
	}
	
	void RenderDevice::AssembleLODSG() {
		for (uint32_t i = 0; i < LODQueryContainers.size(); i++) {
			LODQueryContainer* cont = &LODQueryContainers[i];
			IRenderableActor* pActor = cont->pActor;
			
			// instance is not visiable, so we cull it
			if (cont->pixelCount == 0)
				continue;
			
			// calculate distance bounding sphere border to camera
			Eigen::Vector3f Translation = Eigen::Vector3f(cont->transform.data()[12], cont->transform.data()[13], cont->transform.data()[14]);
			float aabbRadius = std::max(std::abs(pActor->getAABB().Max.norm()), std::abs(pActor->getAABB().Min.norm()));
			float distance = (Translation - m_pActiveCamera->position()).norm() - aabbRadius;
			
			// set pixel count to max if cam is inside BB, due to backface culling
			if (distance < 0.0)
				cont->pixelCount = UINT32_MAX;
			
			// sets LOD level, and transform matrix when instanced
			cont->pActor->evaluateQueryResult(cont->transform, cont->pixelCount);
			
			// TODO push back sorted for front to back rendering
			if (!cont->pActor->isInLODSG()) {
				if (cont->pActor->isInstanced()) {
					m_LODSGTransformations.push_back(Eigen::Matrix4f::Identity());
				}
				else {
					m_LODSGTransformations.push_back(cont->transform);
				}
				cont->pActor->setLODSG(true);
				m_LODSGActors.push_back(cont->pActor);
			}
		}
		LODQueryContainers.clear();
	}
	
	void RenderDevice::LODSGPushBack(IRenderableActor* pActor, Eigen::Matrix4f mat) {
		m_LODSGActors.push_back(pActor);
		m_LODSGTransformations.push_back(mat);
	}
	
}//name space