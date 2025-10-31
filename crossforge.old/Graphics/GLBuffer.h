/*****************************************************************************\
*                                                                           *
* File(s): GLBuffer.h and GLBuffer.cpp                                      *
*                                                                           *
* Content: Wrapper class for various OpenGL buffer objects.                 *
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
#ifndef __CFORGE_GLBUFFER_H__
#define __CFORGE_GLBUFFER_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	/**
	* \brief Wrapper class for various OpenGL buffer objects.
	* \ingroup Graphics
	*/
	class CFORGE_API GLBuffer: public CForgeObject {
	public:
		/**
		* \brief Enumerate of the supported buffer types.
		*/
		enum BufferType: int32_t {
			BTYPE_UNKNOWN = -1,		///< Default value.
			BTYPE_VERTEX = 0,		///< Vertex buffer.
			BTYPE_INDEX,			///< Index buffer.
			BTYPE_SHADER_STORAGE,	///< Shader storage buffer.
			BTYPE_UNIFORM,			///< Uniform buffer.
			BTYPE_TEXTURE,			///< Texture buffer.
		};

		/**
		* \brief Buffer usage enumerate.
		*/
		enum BufferUsage : int32_t {
			BUSAGE_UNKNOWN = -1,	///< Default value.
			BUSAGE_STATIC_DRAW = 0,	///< Seldom read by client.
			BUSAGE_DYNAMIC_DRAW,	///< Regularly rad by client.
		};

		/**
		* \brief Constructor
		*/
		GLBuffer(void);

		/**
		* \brief Destructor
		*/
		~GLBuffer(void);

		/**
		* \brief Initialization method.
		* 
		* \param[in] Type Which type of buffer to create.
		* \param[in] Usage How the buffer is anticipated to be used.
		* \param[in] pBufferData Initial data. Can be nullptr.
		* \param[in] BufferSize Size of pBufferData in bytes.
		*/
		void init(BufferType Type, BufferUsage Usage, const void* pBufferData = nullptr, uint32_t BufferSize = 0);

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Bind the buffer.
		*/
		void bind(void);

		/**
		* \brief Bind buffer base to a binding point retrieved from the shader.
		* 
		* \param[in] BindingPoint Valid binding point retrieved from the shader.
		*/
		void bindBufferBase(uint32_t BindingPoint);

		/**
		* \brief If buffer is a texture buffer this method can bind it.
		* 
		* \param[in] ActiveTexture Texture layer to bind to.
		* \param[in] Format How the texture data should be interpreted. Use OpenGL defines such as GL_RGB
		*/
		void bindTextureBuffer(uint32_t ActiveTexture, uint32_t Format);

		/**
		* \brief Un-binds the buffer.
		*/
		void unbind(void);

		/**
		* \brief Getter of the buffer's type.
		* 
		* \return Buffer type.
		*/
		BufferType type(void)const;

		/**
		* \brief Set new buffer data.
		* 
		* \param[in] pBufferData Data to write.
		* \param[in] BufferSize Size of the data in bytes.
		*/
		void bufferData(const void* pBufferData, uint32_t BufferSize);

		/**
		* \brief Set new data for a part of the buffer.
		* 
		* \param[in] Offset The offset in bytes from the beginning of the buffer.
		* \param[in] Payload Size in bytes of the buffer data.
		* \param[in] pData Data buffer. Has to be at least the size of Payload.
		*/
		void bufferSubData(uint32_t Offset, uint32_t Payload, const void* pData);

		/**
		* \brief Getter for the size of the buffer.
		* 
		* \return Size of the buffer in bytes.
		*/
		uint32_t size(void)const;


	protected:
		BufferType m_BufferType;	///< Buffer type.
		uint32_t m_GLID;			///< OpenGL ID
		uint32_t m_GLTarget;		///< OpenGL target.
		uint32_t m_GLUsage;			///< OpenGL usage.
		uint32_t m_BufferSize;		///< Size in bytes
		uint32_t m_TextureHandle;	///< Texture handle for texture buffers.

	private:

	};//GLBuffer

}//name space

#endif