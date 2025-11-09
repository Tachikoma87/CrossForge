/*****************************************************************************\
*                                                                           *
* File(s): Image2DController.h and Image2DController.cpp                       *
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
#ifndef __CROSSFORGE_IMAGE2DCONTROLLER_H__
#define __CROSSFORGE_IMAGE2DCONTROLLER_H__

#include <crossforge/ecs/ControllerBase.h>
#include <crossforge/assetio/entities/Image2DEntity.h>

namespace crossforge {
	class Image2DController : public ControllerBase {
	public:
		static inline std::string identification = "Image2DController";

		enum BasicImage : int8_t {
			BASIC_IMAGE_UNKNOWN = -1,
			BASIC_IMAGE_8X8_RED = 0,
			BASIC_IMAGE_8X8_GREEN,
			BASIC_IMAGE_8X8_BLUE,
			BASIC_IMAGE_8X8_WHITE,
			BASIC_IMAGE_8X8_BLACK,
			BASIC_IMAGE_COUNT
		};

		static bool flipRows(Image2DEntityPtr pImage2D);
		static bool rotate90Degree(Image2DEntityPtr pImage2D);
		static bool rotate180Degree(Image2DEntityPtr pImage2D);
		static bool rotate270Degree(Image2DEntityPtr pImage2D);
		static bool generateImage(Image2DEntityPtr pImage2D, uint32_t width, uint32_t height, Eigen::Vector3f color);

		static bool resize(Image2DEntityPtr pImage2D, uint32_t width, uint32_t height);
		static bool changeColorSpace(Image2DEntityPtr pimage2D, RawImage2DDataComponent::ColorSpace colorSpace);

		static bool generateBasicImage(Image2DEntityPtr pImage2D, BasicImage basicImage);
		static Image2DEntityPtr generateBasicImage(BasicImage basicImage);

	protected:
		Image2DController(const std::string childIdentification);
		~Image2DController();
	};

	typedef std::shared_ptr<Image2DController> Image2DControllerPtr;
}

#endif