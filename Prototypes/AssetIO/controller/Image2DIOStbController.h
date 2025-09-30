/*****************************************************************************\
*                                                                           *
* File(s): Image2DIOStbController.h and Image2DIOStbController.cpp      *
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
#ifndef __CROSSFORGE_IMAGE2DIOSTBCONTROLLER_H__
#define __CROSSFORGE_IMAGE2DIOSTBCONTROLLER_H__

#include "Image2DIOControllerBase.h"

namespace crossforge {
	class Image2DIOStbController : public Image2DIOControllerBase {
	public:
		inline static std::string identification = "Image2DIOStbController";

		virtual bool load(Image2DEntityPtr pImageEntity, const std::string filepath);
		virtual bool store(Image2DEntityPtr pImageEntity, const std::string fielpath);
		virtual bool canAcceptFile(std::string filePath, const Operation operation)const;

		Image2DIOStbController();
		~Image2DIOStbController();

	protected:
		Image2DIOStbController(const std::string identification);

	};

	typedef std::shared_ptr<Image2DIOStbController> Image2DIOStbControllerPtr;
}

#endif 