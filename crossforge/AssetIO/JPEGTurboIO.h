/*****************************************************************************\
*                                                                           *
* File(s): JPEGTurboIO.h and JPEGTurboIO.cpp                                            *
*                                                                           *
* Content: Import/Export class for jpeg images using libjpeg-turbo   *
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
#ifndef __CFORGE_JPEGTURBOIO_H__
#define __CFORGE_JPEGTURBOIO_H__

#include <crossforge/AssetIO/I2DImageIO.h>

namespace CForge {
	class JPEGTurboIO : public I2DImageIO {
	public:
		JPEGTurboIO();
		~JPEGTurboIO();

		void init();
		void clear();

		void load(const std::string Filepath, T2DImage<uint8_t>* pImgData);
		void load(const uint8_t* pBuffer, const uint32_t BufferLength, T2DImage<uint8_t>* pImgData);
		void store(const std::string Filepath, const T2DImage<uint8_t>* pImgData);
		bool accepted(const std::string Filepath, Operation Op);
		void release(void);

	protected:
		int32_t pixelFormat(const T2DImage<uint8_t>::ColorSpace CS);
		T2DImage<uint8_t>::ColorSpace pixelFormat(int32_t CS);
	};//JPEGTurboIO

}//name-space

#endif 