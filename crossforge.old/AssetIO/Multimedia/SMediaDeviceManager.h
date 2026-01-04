/*****************************************************************************\
*                                                                           *
* File(s): SMediaDeviceManager.h and SMediaDeviceManager.cpp                *
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
#ifndef __CFORGE_SMEDIADEVICEMANAGER_H__
#define __CFORGE_SMEDIADEVICEMANAGER_H__

#ifdef _WIN32
#include <crossforge/Core/CForgeObject.h>
#include "CameraDevice.h"

namespace CForge {
	class SMediaDeviceManager {
	public:
		static SMediaDeviceManager* instance();
		void release();

		int32_t cameraCount()const;
		CameraDevice* camera(int32_t ID);

	protected:
		
		struct Camera {
			int32_t ID;
			std::string Name;
			void* pDeviceHandle;
			CameraDevice* pCamera;

			Camera() {
				ID = -1;
				Name = "";
				pDeviceHandle = nullptr;
			}//Constructor

		};//Camera

		SMediaDeviceManager();
		~SMediaDeviceManager();

		void clear();
		void init();

		void enumerateMediaDevices();
		void enumerateCameraDevices();
	private:
		static SMediaDeviceManager* m_pInstance;
		static uint32_t m_InstanceCount;

		std::vector<Camera*> m_Cameras;

	};//SMediaDeviceManager

}//name space

#endif

#endif 