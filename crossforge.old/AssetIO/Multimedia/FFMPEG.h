/*****************************************************************************\
*                                                                           *
* File(s): FFMPEG.h and FFMPEG.cpp                *
*                                                                           *
* Content: Test file for ffmpeg library   *
*                        *
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
#ifndef __CFORGE_FFMPEG_H__
#define __CFORGE_FFMPEG_H__

#include <crossforge/AssetIO/T2DImage.hpp>

namespace CForge {
	class FFMPEG {
	public:
		FFMPEG();
		~FFMPEG();

		void firstTest();
		void convertNV12(uint8_t* pImgData, uint32_t BufferSize, uint32_t Width, uint32_t Height, T2DImage<uint8_t>* pDest);

	protected:

	};//FMPEG

}//name space

#endif 