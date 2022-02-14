/*****************************************************************************\
*                                                                           *
* File(s): GLBuffer.h and GLBuffer.cpp                                     *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_GLBUFFER_H__
#define __CFORGE_GLBUFFER_H__

#include "../Core/CForgeObject.h"

namespace CForge {
	/**
	* \brief OpenGL buffer object used to store various data in video memory.
	*
	* \todo Do full documentation.
	*/
	class CFORGE_API GLBuffer: public CForgeObject {
	public:
		enum BufferType: int32_t {
			BTYPE_UNKNOWN = -1,
			BTYPE_VERTEX = 0,
			BTYPE_INDEX,
			BTYPE_SHADER_STORAGE,
			BTYPE_UNIFORM,
			BTYPE_TEXTURE,
		};

		enum BufferUsage : int32_t {
			BUSAGE_UNKNOWN = -1,
			BUSAGE_STATIC_DRAW = 0,
			BUSAGE_DYNAMIC_DRAW,
		};

		GLBuffer(void);
		~GLBuffer(void);

		void init(BufferType Type, BufferUsage Usage, const void* pBufferData = nullptr, uint32_t BufferSize = 0);
		void clear(void);

		void bind(void);
		void bindBufferBase(uint32_t BindingPoint);
		void bindTextureBuffer(uint32_t ActiveTexture, uint32_t Format);
		void unbind(void);

		BufferType type(void)const;

		void bufferData(const void* pBufferData, uint32_t BufferSize);
		void bufferSubData(uint32_t Offset, uint32_t Payload, const void* pData);
		uint32_t size(void)const;


	protected:
		BufferType m_BufferType;
		uint32_t m_GLID;
		uint32_t m_GLTarget;
		uint32_t m_GLUsage;
		uint32_t m_BufferSize; ///< Size in bytes
		uint32_t m_TextureHandle; ///< In case of texture buffer

	private:

	};//GLBuffer

}//name space

#endif