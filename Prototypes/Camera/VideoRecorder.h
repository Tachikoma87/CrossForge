/*****************************************************************************\
*                                                                           *
* File(s): VideoRecorder.h and VideoRecorder.cpp                            *
*                                                                           *
* Content:    *
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
#ifndef __CFORGE_VIDEORECORDER_H__
#define __CFORGE_VIDEORECORDER_H__

#include <crossforge/AssetIO/File.h>
#include <crossforge/Core/CForgeObject.h>
#include <crossforge/AssetIO/T2DImage.hpp>
#include <list>

namespace CForge {
	class VideoRecorder : public CForgeObject {
	public:
		VideoRecorder(void);
		~VideoRecorder(void);

		void init();

		void startRecording(const std::string Filename, uint32_t Width, uint32_t Height, const float FPS = 30.0f);
		void stopRecording();
		void clear(void);
		void release(void);

		void addFrame(const T2DImage<uint8_t>* pImg, uint64_t Timestamp);
		void finish(void);

		float fps(void)const;
		uint32_t width(void)const;
		uint32_t height(void)const;

		bool isRecording(void)const;

	protected:
		struct VideoData* m_pData;
	};//VideoRecorder
		
}//name-space

#endif 
