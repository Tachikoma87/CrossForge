/*****************************************************************************\
*                                                                           *
* File(s): WebPImageIO.h and WebPImageIO.cpp                                            *
*                                                                           *
* Content: Import/Export class using libwebp.   *
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
#ifndef __CFORGE_WEBPIMAGEIO_H__
#define __CFORGE_WEBPIMAGEIO_H__

#include "I2DImageIO.h"

namespace CForge {
	class WebPImageIO: public I2DImageIO {
	public:
		WebPImageIO(void);
		~WebPImageIO(void);

		void init(void);
		void clear(void);

		void load(const std::string Filepath, T2DImage<uint8_t>* pImgData);
		void store(const std::string Filepath, const T2DImage<uint8_t>* pImgData);
		bool accepted(const std::string Filepath, Operation Op);
		void release(void);

	protected:

	};//WebPImageIO

}//name space


#endif 