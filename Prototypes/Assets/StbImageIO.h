/*****************************************************************************\
*                                                                           *
* File(s): StbImageIO.h and StbImageIO.cpp                                            *
*                                                                           *
* Content: Import/Export class using stb image.   *
*                                                   *
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
#ifndef __CFORGE_STBIMAGEIO_H__
#define __CFORGE_STBIMAGEIO_H__

#include <CForge/AssetIO/I2DImageIO.h>

namespace CForge {
	class StbImageIO /* : public I2DImageIO*/ {
	public:
		StbImageIO(void);
		~StbImageIO(void);

		void load(const std::string Filepath, T2DImage<uint8_t>* pImgData);
		void store(const std::string Filepath, const T2DImage<uint8_t>* pImgData);
		//bool accepted(const std::string Filepath, Operation Op);
		void release(void);

	protected:

	};//StbImageIO

}//name space


#endif 