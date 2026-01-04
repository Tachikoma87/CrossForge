/*****************************************************************************\
*                                                                           *
* File(s): CameraDevice.h and CameraDevice.cpp                *
*                                                                           *
* Content:    *
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
#ifndef __CFORGE_CAMERADEVICE_H__
#define __CFORGE_CAMERADEVICE_H__

#include <crossforge/Core/CForgeObject.h>
#include <crossforge/AssetIO/T2DImage.hpp>

namespace CForge {
	class CameraDevice {
	public:
		struct CaptureFormat {
			Eigen::Vector2i FrameSize;
			std::string DataFormat;
			uint32_t StreamIndex;
			std::vector<uint32_t> FPS;
		};

		CameraDevice();
		~CameraDevice();

		void init(void *pDeviceHandle);
		void clear();
		void release();

		uint32_t captureFormatCount()const;
		CaptureFormat captureFormat(int32_t ID)const;
		void findOptimalCaptureFormats(int32_t Width, int32_t Height, std::vector<int32_t> *pFormatIDs);
		void changeCaptureFormat(int32_t FormatID, int32_t FPS = -1);

		void retrieveImage(T2DImage<uint8_t>* pImg);

	protected:
		void enumerateCaptureFormats();
		

		void* m_pDeviceHandle;

		std::vector<CaptureFormat> m_CaptureFormats;
		int32_t m_ActiveCaptureFormat;
	};//CameraDevice

}//name space

#endif 