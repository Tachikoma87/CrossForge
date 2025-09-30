/*****************************************************************************\
*                                                                           *
* File(s): Image2DIOControllerBase.h and Image2DIOControllerBase.cpp     *
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
#ifndef __CROSSFORGE_IMAGE2DIOCONTROLLERBASE_H__
#define __CROSSFORGE_IMAGE2DIOCONTROLLERBASE_H__

#include "../../ECS/ControllerBase.h"
#include "../entities/Image2DEntity.h"

namespace crossforge {
	class Image2DIOControllerBase : public ControllerBase {
	public:
		inline static std::string identification = "Image2DIOControllerBase";

		enum Operation : uint8_t {
			OP_LOAD = 0,	///< Loading.
			OP_STORE,		///< Storing.
		};

		virtual bool load(Image2DEntityPtr pImageEntity, const std::string filepath) = 0;
		virtual bool store(Image2DEntityPtr pImageEntity, const std::string fielpath) = 0;
		virtual bool canAcceptFile(std::string filePath, const Operation operation)const = 0;


		~Image2DIOControllerBase();
	protected:

		Image2DIOControllerBase(const std::string childIdentification);

	};

	typedef std::shared_ptr<Image2DIOControllerBase> Image2DIOControllerBasePtr;
}

#endif 