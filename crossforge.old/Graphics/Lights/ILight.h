/*****************************************************************************\
*                                                                           *
* File(s): ILight.h and ILight.cpp                                *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CFORGE_ILIGHT_H__
#define __CFORGE_ILIGHT_H__

#include "../Shader/GLShader.h"
#include "../../AssetIO/T2DImage.hpp"
#include "../../Core/ITCaller.hpp"
#include "../Camera/VirtualCamera.h"

namespace CForge {

	struct LightMsg {
		enum MsgCode {
			MC_POSITION_CHANGED = 0,
			MC_DIRECTION_CHANGED,
			MC_COLOR_CHANGED,
			MC_INTENSITY_CHANGED,
			MC_SHADOW_CHANGED,
			MC_DESTROYED,
		};

		MsgCode Code;
		void* pHandle; // of type ILight

	};//LightMsg

	/**
	* \brief Basic interface for all light classes. Implements basic properties and shadow casting.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API ILight: public CForgeObject, public ITCaller<LightMsg> {
	public:
		enum LightType: int8_t {
			LIGHT_UNKNOWN = -1,
			LIGHT_DIRECTIONAL = 0,
			LIGHT_POINT,
			LIGHT_SPOT,
		};

		virtual void init(const Eigen::Vector3f Pos, const Eigen::Vector3f Dir, const Eigen::Vector3f Color, float Intensity);
		virtual void initShadowCasting(uint32_t ShadowMapWidth, uint32_t ShadowMapHeight, Eigen::Vector2i ViewDimension, float NearPlane, float FarPlane);
		
		virtual void clear(void);

		virtual void position(Eigen::Vector3f Pos);
		virtual void direction(Eigen::Vector3f Dir, bool Normalize = false);
		virtual void color(Eigen::Vector3f Color);
		virtual void intensity(float Intensity);

		virtual Eigen::Vector3f position(void)const;
		virtual Eigen::Vector3f direction(void)const;
		virtual Eigen::Vector3f color(void)const;
		virtual float intensity(void)const;
		virtual LightType type(void)const;

		virtual void bindShadowFBO(void);
		virtual  void bindShadowTexture(GLShader* pShader, GLShader::DefaultTex ShadowLevel);
		virtual Eigen::Matrix4f viewMatrix(void)const;
		virtual Eigen::Matrix4f projectionMatrix(void)const;
		virtual void retrieveDepthBuffer(T2DImage<uint8_t>* pImg);
		virtual bool castsShadows(void)const;
		virtual Eigen::Vector2i shadowMapSize(void)const;
		virtual const VirtualCamera *camera(void)const;

	protected:
		ILight(LightType T, const std::string ClassName);
		~ILight(void);

		virtual void initShadowCasting(uint32_t ShadowMapWidth, uint32_t ShadowMapHeight, Eigen::Matrix4f Projection);

		//Eigen::Vector3f m_Position;
		//Eigen::Vector3f m_Direction;
		Eigen::Vector3f m_Color;
		float m_Intensity;
		LightType m_Type;

		uint32_t m_ShadowTex;
		uint32_t m_FBO; ///< Framebuffer object
		Eigen::Vector2i m_ShadowMapSize;

		//Eigen::Matrix4f m_Projection;
		LightMsg m_Msg;
		VirtualCamera m_Camera;
	};//ILight

}//name space


#endif 