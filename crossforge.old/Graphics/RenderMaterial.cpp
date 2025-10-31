#include "RenderMaterial.h"
#include "Textures/STextureManager.h"
#include "../Core/SLogger.h"

namespace CForge {

	RenderMaterial::RenderMaterial(void): CForgeObject("RenderMaterial") {
		m_pAlbedoMap = nullptr;
		m_pNormalMap = nullptr;
		m_pDepthMap = nullptr;
	}//Constructor

	RenderMaterial::~RenderMaterial(void) {
		clear();
	}//Destructor

	void RenderMaterial::init(const T3DMesh<float>::Material *pMat) {
		clear();
		if (nullptr == pMat) throw NullpointerExcept("pMat");

		
		if (!pMat->TexAlbedo.empty()) {
			try {
			m_pAlbedoMap = STextureManager::create(pMat->TexAlbedo);
			}
			catch (const CrossForgeException& e) {
				SLogger::logException(e);
				m_pAlbedoMap = STextureManager::create(8, 8, uint8_t(255U), uint8_t(255U), uint8_t(255U), false);
			}
		}
		else m_pAlbedoMap = STextureManager::create(8, 8, uint8_t(255U), uint8_t(255U), uint8_t(255U), false);
		
			
		if (!pMat->TexNormal.empty()) {
			try {
				m_pNormalMap = STextureManager::create(pMat->TexNormal);
			}
			catch (const CrossForgeException& e) {
				SLogger::logException(e);
				m_pNormalMap = STextureManager::create(8, 8, uint8_t(0), uint8_t(0), uint8_t(255U), false);
			}
		}

		if (!pMat->TexDepth.empty()) {
			try {
				m_pDepthMap = STextureManager::create(pMat->TexDepth);
			}catch (const CrossForgeException& e) {
				SLogger::logException(e);
				m_pDepthMap = STextureManager::create(8, 8, uint8_t(0), uint8_t(0), uint8_t(0), false);
			}
			
		}

		m_Color = pMat->Color;
		m_Metallic = pMat->Metallic;
		m_Roughness = pMat->Roughness;
		m_AmbientOcclusion = 1.0f;
	}//initialize

	void RenderMaterial::clear(void) {
		if (nullptr != m_pAlbedoMap) STextureManager::destroy(m_pAlbedoMap);
		if (nullptr != m_pNormalMap) STextureManager::destroy(m_pNormalMap);
		if (nullptr != m_pDepthMap) STextureManager::destroy(m_pDepthMap);
		m_pAlbedoMap = nullptr;
		m_pNormalMap = nullptr;
		m_pDepthMap = nullptr;

		m_Color = Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		m_Metallic = 0.04f;
		m_Roughness = 0.5f;
		m_AmbientOcclusion = 0.05f;
	}//clear

	GLTexture2D* RenderMaterial::albedoMap(void)const {
		return m_pAlbedoMap;
	}//albedoMap

	GLTexture2D* RenderMaterial::normalMap(void)const {
		return m_pNormalMap;
	}//normalMap

	GLTexture2D* RenderMaterial::depthMap(void)const {
		return m_pDepthMap;
	}//depthMap

	const Eigen::Vector4f RenderMaterial::color(void)const {
		return m_Color;
	}//color

	const float RenderMaterial::metallic(void)const {
		return m_Metallic;
	}//metallic

	const float RenderMaterial::roughness(void)const {
		return m_Roughness;
	}//roughness

	const float RenderMaterial::ambientOcclusion(void)const {
		return m_AmbientOcclusion;
	}//ambientOcclusion

	void RenderMaterial::color(const Eigen::Vector4f Color) {
		m_Color = Color;
	}//color

	void RenderMaterial::metallic(float Metallic) {
		m_Metallic = Metallic;
	}//metallic

	void RenderMaterial::roughness(float Roughness) {
		m_Roughness = Roughness;
	}//roughness

	void RenderMaterial::ambientOcclusion(float Ao) {
		m_AmbientOcclusion = Ao;
	}//ambientOccclusion
}//name space