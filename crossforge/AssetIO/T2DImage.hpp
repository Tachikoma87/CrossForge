/*****************************************************************************\
*                                                                           *
* File(s): T2DImage.hpp                                     *
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
#ifndef __CFORGE_T2DIMAGE_HPP__
#define __CFORGE_T2DIMAGE_HPP__

#include "../Core/CForgeObject.h"

namespace CForge {

	/**
	* \brief Template class that stores a 2D image in different formats.
	* 
	* \todo Do full documentation
	* \todo Add compressed formats (DDS)
	*
	*/
	template <typename T>
	class T2DImage {
	public:
		enum ColorSpace : int8_t {
			COLORSPACE_UNKNOWN = -1,
			COLORSPACE_GRAYSCALE = 0,
			COLORSPACE_RGB,
			COLORSPACE_RGBA,
		};//ColorSpace


		T2DImage(uint32_t Width = 0, uint32_t Height = 0, ColorSpace CS = COLORSPACE_UNKNOWN, const T* pData = nullptr) {
			m_Width = 0;
			m_Height = 0;
			m_pData = nullptr;
			m_ColorSpace = COLORSPACE_UNKNOWN;
			init(Width, Height, CS, pData);
		}//Constructor

		~T2DImage(void) {
			clear();
		}//Destructor

		void init(uint32_t Width, uint32_t Height, ColorSpace CS, const T* pData) {
			clear();

			if (Width != 0 && Height != 0) {
				m_Width = Width;
				m_Height = Height;
				m_ColorSpace = CS;
				m_pData = new T[pixelCount()*componentsPerPixel()];
				if (nullptr == m_pData) {
					clear();
					throw OutOfMemoryExcept("m_pData");
				}
				if (nullptr != pData) memcpy(m_pData, pData, size());
			}//if[initializing]

		}//initialize

		void clear(void) {
			delete[] m_pData;
			m_pData = nullptr;
			m_Width = 0;
			m_Height = 0;
			m_ColorSpace = COLORSPACE_UNKNOWN;
		}//clear

		uint8_t componentsPerPixel(void)const {
			uint8_t Rval = 0;
			switch (m_ColorSpace) {
			case COLORSPACE_GRAYSCALE: Rval = 1; break;
			case COLORSPACE_RGB: Rval = 3; break;
			case COLORSPACE_RGBA: Rval = 4; break;
			default: {
				Rval = 0;
			}break;
			}//switch[ColorSpace]

			return Rval;
		}//componentsPerPixel

		uint32_t pixelCount(void)const {
			return m_Width * m_Height;
		}//pixelCount

		uint32_t width(void)const {
			return m_Width;
		}//width

		uint32_t height(void)const {
			return m_Height;
		}//height

		uint32_t size(void) {
			return pixelCount() * componentsPerPixel() * sizeof(T);
		}//size

		const T* pixel(uint32_t x, uint32_t y) const{
			return &m_pData[index(x,y)];
		}//pixel

		T* pixel(uint32_t x, uint32_t y) {
			return &m_pData[index(x, y)];
		}//pixel

		const T* data(void)const {
			return m_pData;
		}//data

		T* data(void) {
			return m_pData;
		}//data

		ColorSpace colorSpace(void)const {
			return m_ColorSpace;
		}//colorSpace

		void flipRows(void) {
			T* pNewData = new T[m_Width * m_Height * componentsPerPixel()];
			uint32_t RowSize = m_Width * componentsPerPixel();
			for (uint32_t i = 0; i < m_Height; ++i) {
				uint32_t IndexOrig = i * RowSize;
				uint32_t IndexNew = (m_Height - i - 1) * RowSize;
				memcpy(&pNewData[IndexNew], &m_pData[IndexOrig], RowSize * sizeof(T));
			}
			delete[] m_pData;
			m_pData = pNewData;
		}//flipRows

		void rotate90(void) {
			T* pNewData = new T[m_Width * m_Height * componentsPerPixel()];

			for (uint32_t r = 0; r < m_Width; ++r) {
				for (uint32_t c = 0; c < m_Height; ++c) {
					for (uint8_t p = 0; p < componentsPerPixel(); ++p)pNewData[(c * m_Height + (m_Height - r - 1))*componentsPerPixel() + p] = m_pData[(r * m_Width + c)*componentsPerPixel() + p];
				}
			}

			delete[] m_pData;
			m_pData = pNewData;

			std::swap(m_Width, m_Height);
		}//rotate90

		void rotate180(void) {
			rotate90();
			rotate90();
		}//rotate180

		void rotate270(void) {
			rotate90();
			rotate90();
			rotate90();
		}//rotate270

	protected:

		uint32_t index(uint32_t x, uint32_t y) const {
			if (x >= m_Width || y >= m_Height) throw IndexOutOfBoundsExcept("x or y");
			return (y * m_Width + x) * componentsPerPixel();
		}//index

		uint32_t m_Width;
		uint32_t m_Height;
		T* m_pData;
		ColorSpace m_ColorSpace;
	};//T2DImage

}//name space

#endif