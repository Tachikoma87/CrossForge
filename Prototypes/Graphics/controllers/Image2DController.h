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

		static bool fliprRows(Image2DEntityPtr pImage2D);
		static bool rotate90Degree(Image2DEntityPtr pImage2D);
		static bool rotate180Degree(Image2DEntityPtr pImage2D);
		static bool rotate270Degree(Image2DEntityPtr pImage2D);

	protected:
		Image2DController(const std::string childIdentification);
		~Image2DController();
	};

	typedef std::shared_ptr<Image2DController> Image2DControllerPtr;
}

#endif