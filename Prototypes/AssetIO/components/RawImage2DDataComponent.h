/*****************************************************************************\
*                                                                           *
* File(s): RawImage2DDataComponent.h and RawImage2DDataComponent.cpp        *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_RAWIMAGE2DDATACOMPONENT_H__
#define __CROSSFORGE_RAWIMAGE2DDATACOMPONENT_H__

#include <crossforge/ecs/ComponentBase.h>

namespace crossforge {
	class RawImage2DDataComponent : public ComponentBase {
	public:
		inline static std::string identification = "RawImage2DDataComponent";

		enum ColorSpace : int8_t {
			COLORSPACE_UNKNOWN = -1,	///< Default value.
			COLORSPACE_GRAYSCALE = 0,	///< Grayscale.
			COLORSPACE_RGB,				///< Red-Green-Blue (RGB).
			COLORSPACE_RGBA,			///< Red-green-blue-alpha (RGBA).
		};//ColorSpace

		RawImage2DDataComponent();
		~RawImage2DDataComponent();

		void initialize();
		void clear();

		int32_t& width();
		int32_t& height();
		ColorSpace& colorSpace();
		std::vector<uint8_t>& rawPixelData();


		int8_t getBitsPerPixel()const;
		int8_t getBytesPerPixel()const;
		uint64_t getImageSize()const;

	protected:
		int32_t m_width;
		int32_t m_height;
		ColorSpace m_colorSpace;
		std::vector<uint8_t> m_rawPixelData;
	};

	typedef std::shared_ptr<RawImage2DDataComponent> RawImage2DDataComponentptr;
}

#endif