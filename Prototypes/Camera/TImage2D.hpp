/*****************************************************************************\
*                                                                           *
* File(s): Image2D.h and Image2D.cpp                                              *
*                                                                           *
* Content:    *
*                                                   *
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

#ifndef __CFORGE_TIMAGE2D_HPP__
#define __CFORGE_TIMAGE2D_HPP__

#include <crossforge/Core/CrossForgeException.h>
#include <string>

namespace CForge {


	template<typename T>
	class TImage2D {
	public:
		enum ColorMode: uint8_t {
			COLORMODE_UNKNOWN = 0,
			COLORMODE_GRAYSCALE,
			COLORMODE_RGB,
			COLORMODE_RGBA,
			COLORMODE_RGB_DXT1,
			COLORMODE_RGBA_DXT1,
			COLORMODE_RGBA_DXT3,
			COLORMODE_RGBA_DXT5,
			COLORMODE_COUNT,
		};

		TImage2D(void) {

		}//Constructor

		~TImage2D(void) {

		}//Destructor

		void init(uint16_t Width, uint16_t Height, T* pData, ColorMode CMode) {
			clear();
		}//initialize

		void init(const TImage2D<T>* pImg) {
			clear();
			m_Width = pImg->width();
			m_Height = pImg->height();
			m_ColorMode = pImg->colorMode();
			m_Name = pImg->name();
			switch (m_ColorMode) {
			case COLORMODE_GRAYSCALE:
			case COLORMODE_RGB:
			case COLORMODE_RGBA: {
				m_pData = new T[m_Width * m_Height * pImg->componentsPerPixel()];
			}break;
			case COLORMODE_RGB_DXT1:
			case COLORMODE_RGBA_DXT1:
			case COLORMODE_RGBA_DXT3:
			case COLORMODE_RGBA_DXT5: {
				if (sizeof(T) != sizeof(uint8_t)) throw CForgeExcept("DXT1 compressed images must have uint8_t data type!");
				m_pData = new T[pImg->size()];
			}break;
			default: {
				throw CForgeExcept("Image has invalid color mode");
			}break;
			}//switch[color mode]
			
			memcpy(m_pData, pImg->m_pData, pImg->size());
		}//initialize


		void clear(void) {
			delete[] m_pData;
			m_pData = nullptr;
			m_Width = 0;
			m_Height = 0;
			m_Size = 0;
			m_ColorMode = COLORMODE_UNKNOWN;
		}//clear

		uint8_t componentsPerPixel(void)const {
			uint8_t Rval = 0;
			switch (m_ColorMode) {
			case COLORMODE_GRAYSCALE: Rval = 1; break;
			case COLORMODE_RGB: Rval = 3; break;
			case COLORMODE_RGBA: Rval = 4; break;
			case COLORMODE_RGB_DXT1: Rval = 3; break;
			case COLORMODE_RGBA_DXT1: Rval = 4; break;
			case COLORMODE_RGBA_DXT3: Rval = 4; break;
			case COLORMODE_RGBA_DXT5: Rval = 4; break;
			default: {
				throw CForgeExcept("Image has invalid color mode");
			}break;
			}//switch[Color mode]
			return Rval;
		}//componentsPerPixel

		uint32_t size(void)const {
			uint32_t Rval = 0;
			switch (m_ColorMode) {
			case COLORMODE_GRAYSCALE:
			case COLORMODE_RGB:
			case COLORMODE_RGBA: {
				Rval = m_Width * m_Height * componentsPerPixel() * sizeof(T);
			}break;
			case COLORMODE_RGB_DXT1:
			case COLORMODE_RGBA_DXT1:
			case COLORMODE_RGBA_DXT3:
			case COLORMODE_RGBA_DXT5: {
				Rval = m_Size;
			}break;
			default: {
				throw CForgeExcept("Image has invalid color mode");
			}break;
			}//switch[color mode]
			return Rval;
			
		}//size

		void name(const std::string Name) {
			m_Name = Name;
		}//name

		std::string name(void)const {
			return m_Name;
		}//name

		uint16_t width(void)const {
			return m_Width;
		}//width

		uint16_t height(void)const {
			return m_Height;
		}//height

		ColorMode colorMode(void)const {
			return m_ColorMode;
		}//colorMode

		const T* pixelAt(uint16_t x, uint16_t y) const {
			if (x >= m_Width || y >= m_Height) throw IndexOutOfBoundsExcept("x or y");
			uint32_t Index = ((y * m_Width) + x) * componentsPerPixel();
			return &m_pData[Index];
		}//pixel

		void pixelAt(uint16_t x, uint16_t y, const T* pValues) {
			if (x >= m_Width || y >= m_Height) throw IndexOutOfBoundsExcept("x or y");
			uint32_t Index = ((y * m_Width) + x) * componentsPerPixel();
			memcpy(&m_pData[Index], pValues, sizeof(T) * componentsPerPixel());
		}//pixelAt

	protected:
		T* m_pData;
		uint16_t m_Width; ///< Width of the image (number of columns)
		uint16_t m_Height; ///< Height of the image (number of rows)
		uint32_t m_Size; ///< Size in bytes (for compressed images)
		ColorMode m_ColorMode; ///< Color type 
		std::string m_Name; ///< Name of the image

	};//Image2D


	typedef TImage2D<uint8_t> Image2D;
	typedef TImage2D<float> FloatImage2D;

}//name-space


#endif