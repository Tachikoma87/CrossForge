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
#ifdef INCLUDE_OPENCV

#include <crossforge/Core/CForgeObject.h>
#include <crossforge/AssetIO/T2DImage.hpp>
#include <list>

namespace CForge {
	class VideoRecorder : public CForgeObject {
	public:
		VideoRecorder(void);
		~VideoRecorder(void);

		void init(const std::string Filename, const float FPS = 30.0f);
		void clear(void);
		void release(void);

		void addFrame(const T2DImage<uint8_t>* pImg, uint64_t Timestamp);
		void finish(void);

		float fps(void)const;
		uint32_t width(void)const;
		uint32_t height(void)const;

		bool isRecording(void)const;

	protected:
		struct Frame {
			T2DImage<uint8_t> Img;
			uint64_t Timestamp;
		};

		std::list<Frame*> m_FrameBuffer;

		std::string m_Filename;
		float m_FPS;

		uint64_t m_LastTimestamp;
		uint32_t m_FrameWidth;
		uint32_t m_FrameHeight; 

		void* m_pVideoWriter;

	};//VideoRecorder
		
}//name-space

#endif 
#endif