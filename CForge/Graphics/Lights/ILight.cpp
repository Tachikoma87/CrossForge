#include <glad/glad.h>
#include "../GraphicsUtility.h"
#include "ILight.h"

using namespace Eigen;

namespace CForge {

	ILight::ILight(LightType T, const std::string ClassName): CForgeObject("ILight::" + ClassName) {
		m_Type = T;
		m_Position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		m_Direction = Eigen::Vector3f(0.0f, -1.0f, 0.0f);
		m_Color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
		m_Intensity = 1.0f;
		m_ShadowMapSize = Eigen::Vector2i(0, 0);
		m_ShadowTex = GL_INVALID_INDEX;
		m_FBO = GL_INVALID_INDEX;
	}//Constructor

	ILight::~ILight(void) {
		clear();
	}//Destructor

	void ILight::init(const Eigen::Vector3f Pos, const Eigen::Vector3f Dir, const Eigen::Vector3f Color, float Intensity) {
		clear();
		m_Position = Pos;
		m_Direction = Dir.normalized();
		m_Color = Color;
		m_Intensity = Intensity;
	}//initialize

	void ILight::clear(void) {
		m_Position = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
		m_Direction = Eigen::Vector3f(0.0f, -1.0f, 0.0f);
		m_Color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
		m_Intensity = 1.0f;

		if (glIsTexture(m_ShadowTex)) glDeleteTextures(1, &m_ShadowTex);
		if (glIsFramebuffer(m_FBO)) glDeleteFramebuffers(1, &m_FBO);
		m_ShadowTex = GL_INVALID_INDEX;
		m_FBO = GL_INVALID_INDEX;
		m_ShadowMapSize = Vector2i(0, 0);
		m_Projection = Matrix4f::Identity();

	}//clear

	void ILight::position(Eigen::Vector3f Pos) {
		m_Position = Pos;
	}//position

	void ILight::direction(Eigen::Vector3f Dir, bool Normalize) {
		m_Direction = (Normalize) ? Dir.normalized() : Dir;
	}//direction

	void ILight::color(Eigen::Vector3f Color) {
		m_Color = Color;
	}//color

	void ILight::intensity(float Intensity) {
		m_Intensity = Intensity;
	}//intensity

	Eigen::Vector3f ILight::position(void)const {
		return m_Position;
	}//position

	Eigen::Vector3f ILight::direction(void)const {
		return m_Direction;
	}//direction

	Eigen::Vector3f ILight::color(void)const {
		return m_Color;
	}//color

	float ILight::intensity(void)const {
		return m_Intensity;
	}//intensity

	ILight::LightType ILight::type(void)const {
		return m_Type;
	}//type

	void ILight::initShadowCasting(uint32_t Width, uint32_t Height, Eigen::Vector2i ViewDimension, float NearPlane, float FarPlane) {
		Eigen::Matrix4f Projection = GraphicsUtility::orthographicProjection(ViewDimension.x(), ViewDimension.y(), NearPlane, FarPlane);
		initShadowCasting(Width, Height, Projection);

	}//initShadowMap

	void ILight::initShadowCasting(uint32_t ShadowMapWidth, uint32_t ShadowMapHeight, Eigen::Matrix4f Projection) {
		m_ShadowMapSize = Eigen::Vector2i(ShadowMapWidth, ShadowMapHeight);

		//// generate shadow map
		glGenTextures(1, &m_ShadowTex);
		glBindTexture(GL_TEXTURE_2D, m_ShadowTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowMapSize.x(), m_ShadowMapSize.y(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// set border to 1.0f (farthest possible depth value) so that regions outside the shadow map are not in shadow
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float BorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);

		// generate and configure framebuffer object
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_ShadowTex, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_Projection = Projection;
	}//initShadowCasting

	void ILight::bindShadowFBO(void) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}//bindShadowFBO

	Eigen::Matrix4f ILight::viewMatrix(void)const {
		return GraphicsUtility::lookAt(m_Position, m_Position + m_Direction);
	}//viewMatrix

	Eigen::Matrix4f ILight::projectionMatrix(void)const {
		//return GraphicsUtility::orthographicProjection(m_ViewDimension.x(), m_ViewDimension.y(), m_NearPlane, m_FarPlane);
		return m_Projection;
	}//projectionMatrix

	void ILight::retrieveDepthBuffer(T2DImage<uint8_t>* pImg) {
		GraphicsUtility::retrieveDepthTexture(m_ShadowTex, pImg, 0.01f, 50.0f);

	}//retrieveDeptBuffer

	void ILight::bindShadowTexture(GLShader* pShader, GLShader::DefaultTex ShadowLevel) {
		if (nullptr == pShader) throw NullpointerExcept("pShader");

		uint32_t Loc = pShader->uniformLocation(ShadowLevel);
		if (GL_INVALID_INDEX != Loc) {
			glActiveTexture(GL_TEXTURE0 + Loc);
			glBindTexture(GL_TEXTURE_2D, m_ShadowTex);
			glUniform1i(Loc, Loc);
		}
	 }//bindShadowTexture

	bool ILight::castsShadows(void)const {
		return (m_ShadowMapSize.x() != 0 && m_ShadowMapSize.y() != 0);
	}

	Eigen::Vector2i ILight::shadowMapSize(void)const {
		return m_ShadowMapSize;
	}//shadowMapSize


}//name space