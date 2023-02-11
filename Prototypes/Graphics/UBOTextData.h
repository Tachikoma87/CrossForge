/*****************************************************************************\
*                                                                           *
* File(s): UBOTextData.h and UBOTextData.cpp                                    *
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
#ifndef __CFORGE_UBOTEXTDATA_H__
#define __CFORGE_UBOTEXTDATA_H__

#include <CForge/Graphics/GLBuffer.h>


namespace CForge {
	/**
	* \brief Uniform buffer object for camera related data.
	*
	* \todo Do full documentation.
	*/
	class UBOTextData : public CForgeObject {
	public:
		UBOTextData(void);
		~UBOTextData(void);

		void init(void);
		void clear(void);

		void bind(uint32_t BindingPoint);

		void color(const Eigen::Vector4f Color);
		void canvasSize(Eigen::Vector2f CanvasSize);
		void textPosition(Eigen::Vector2f TextPosition);
		uint32_t size(void)const;

	protected:

	private:
		uint32_t m_ColorOffset;
		uint32_t m_CanvasSizeOffset;
		uint32_t m_TextPositionOffset;
		
		GLBuffer m_Buffer;
	};//UBOTextData

}//name space

#endif