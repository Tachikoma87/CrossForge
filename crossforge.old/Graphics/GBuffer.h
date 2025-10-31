/*****************************************************************************\
*                                                                           *
* File(s): GBuffer.h and GBuffer.cpp                                        *
*                                                                           *
* Content: Global buffer for deferred rendering.                            *
*                                                                           *
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
#ifndef __CFORGE_GBUFFER_H__
#define __CFORGE_GBUFFER_H__

#include "../Core/CForgeObject.h"
#include "../AssetIO/T2DImage.hpp"

namespace CForge{ 
	/**
	* \brief Global buffer object. Stores rendering data (position, normal, albedo) and provides it by textures.
	* \ingroup Graphics
	* 
	* \todo Change pass by pointers to pass by reference
	* \todo Finish implementation of multisample frame buffers
	*/
	class CFORGE_API GBuffer: public CForgeObject {
	public:
		/**
		* \brief Enumerates of available global buffer components.
		*/
		enum Component: int8_t {
			COMP_POSITION = 0,	///< Position data in world space.
			COMP_NORMAL,		///< Normal data in world space.
			COMP_ALBEDO,		///< Albedo (RGB) values.
			COMP_DEPTH_STENCIL	///< Depth and stencil values.
		};

		/**
		* \brief Constructor
		*/
		GBuffer(void);

		/**
		* \brief Destructor
		*/
		~GBuffer(void);

		/**
		* \brief Initialization method.
		* 
		* \param[in] Width The intended width of the frame buffer.
		* \param[in] Height The intended height of the frame buffer.
		*/
		void init(uint32_t Width, uint32_t Height);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Binds the frame buffer.
		*/
		void bind(void)const;

		/**
		* \brief Unbinds the frame buffer, i.e. going back to the default frame buffer.
		*/
		void unbind(void)const;

		/**
		* \brief Binds the respective global buffer's component texture.
		* 
		* \param[in] Comp The component to bind.
		* \param[in] Level Texture level to bind to.
		*/
		void bindTexture(Component Comp, uint32_t Level);

		/**
		* \brief Copies the depth buffer component of the frame buffer to the bound frame buffer. Assumes both buffers have the same size.
		* 
		* \param[in] Width The width of the buffers.
		* \param[in] Height The height of the buffers.
		*/
		void blitDepthBuffer(uint32_t Width, uint32_t Height);

		/**
		* \brief Copies the depth buffer component of the frame buffer to the bound frame buffer.
		* 
		* \param[in] SourcePos Start position of the copy operation regarding the source frame buffer.
		* \param[in] SourceSize Width and height of the copy operation regarding the source frame buffer.
		* \param[in] TargetPos Start position of the copy operation regarding the target frame buffer.
		* \param[in] TargetSize Width and height of the copy operation regarding the target frame buffer.
		*/
		void blitDepthBuffer(Eigen::Vector2i SourcePos, Eigen::Vector2i SourceSize, Eigen::Vector2i TargetPos, Eigen::Vector2i TargetSize);

		/**
		* \brief Getter for the width of the frame buffer.
		* 
		* \return Width of the frame buffer.
		*/
		uint32_t width(void)const;

		/**
		* \brief Getter for the height of the frame buffer.
		* 
		* \return Height of the frame buffer.
		*/
		uint32_t height(void)const;

		/**
		* \brief Getter of the content of the position texture.
		* 
		* \param[out] pImg Will contain the global buffer's position data.
		*/
		void retrievePositionBuffer(T2DImage<uint8_t> *pImg);

		/**
		* \brief Getter of the content of the normal texture.
		* 
		* \param[out] pImg Will contain the global buffer's normal data.
		*/
		void retrieveNormalBuffer(T2DImage<uint8_t>* pImg);

		/**
		* \brief Getter of the content of the albedo buffer.
		* 
		* \param[out] pImg Will contain the global buffer's albedo data.
		*/
		void retrieveAlbedoBuffer(T2DImage<uint8_t>* pImg);

		/**
		* \brief Getter of the content of the depth buffer.
		* 
		* \param[out] pImg Will contain the content of the depth buffer.
		* \param[in] Near Near plane's value for depth linearizion.
		* \param[int] Far Far plane's value for depth linearizion
		*/
		void retrieveDepthBuffer(T2DImage<uint8_t>* pImg, float Near = -1.0f, float Far = -1.0f);

	protected:
		uint32_t m_Framebuffer;		///< GL identifier of the frame buffer.
		uint32_t m_Renderbuffer;	///< GL identifier of the render buffer.

		uint32_t m_TexPosition;		///< GL texture object of position data.
		uint32_t m_TexNormal;		///< GL texture object of normal data.
		uint32_t m_TexAlbedo;		///< GL texture object of albedo data.
		uint32_t m_TexDepthStencil;	///< GL texture object of depth, stencil data.

		uint32_t m_Width;		///< Width of the frame buffer.
		uint32_t m_Height;		///< Height of the frame buffer.
	};//GBuffer

}//name space


#endif 