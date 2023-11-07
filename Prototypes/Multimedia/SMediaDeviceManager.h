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

#include <crossforge/Core/CForgeObject.h>

namespace CForge {
	class SMediaDeviceManager {
	public:
		static SMediaDeviceManager* instance();
		void release();



	protected:
		struct CameraDevice {
			std::string Name;

		};//CameraDevice

		SMediaDeviceManager();
		~SMediaDeviceManager();

		void clear();
		void init();
	private:
		static SMediaDeviceManager* m_pInstance;
		static uint32_t m_InstanceCount;

	};//SMediaDeviceManager

}//name space

#endif 