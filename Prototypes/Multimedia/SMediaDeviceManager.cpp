#ifdef _WIN32

#include <crossforge/Core/SLogger.h>
#include <crossforge/Utility/CForgeUtility.h>

#include "SMediaDeviceManager.h"

#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <Mferror.h>

#pragma comment(lib, "dxva2.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace CForge {
	SMediaDeviceManager* SMediaDeviceManager::m_pInstance = nullptr;
	uint32_t SMediaDeviceManager::m_InstanceCount = 0;

	SMediaDeviceManager* SMediaDeviceManager::instance() {
		if (nullptr == m_pInstance) {
			m_pInstance = new SMediaDeviceManager();
		}
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SMediaDeviceManager::release() {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (0 == m_InstanceCount) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	SMediaDeviceManager::SMediaDeviceManager() {
		// windows specific initialization calls
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (S_OK != MFStartup(MF_VERSION)) throw CForgeExcept("Initializing media foundation failed\n");
		init();
	}//Constructor

	SMediaDeviceManager::~SMediaDeviceManager() {

	}//Destructor

	void SMediaDeviceManager::clear() {

	}//clear

	void SMediaDeviceManager::init() {
		clear();
		enumerateMediaDevices();
	}//initialize

	void SMediaDeviceManager::enumerateMediaDevices() {
		enumerateCameraDevices();
	}//enumerateMediaDevices

	void SMediaDeviceManager::enumerateCameraDevices() {
		uint32_t Count = 0;

		IMFAttributes* pConfig = nullptr;
		IMFActivate** ppDevices = nullptr;
		IMFMediaSource* pSource = nullptr;
		IMFSourceReader* pReader = nullptr;

		// Create an attribute store to hold the search criteria
		HRESULT hr = MFCreateAttributes(&pConfig, 1);

		// Request video capture devices
		if (SUCCEEDED(hr)) {
			hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		}
		else {
			throw CForgeExcept("Failed to create MF attributes!");
		}
		hr = MFEnumDeviceSources(pConfig, &ppDevices, &Count);
		if (SUCCEEDED(hr)) {

			for (uint32_t i = 0; i < Count; ++i) {	
				// retrieve name
				WCHAR* szFriendlyName = nullptr;
				uint32_t cchName;
				hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &szFriendlyName, &cchName);
				std::string DeviceName = CForgeUtility::convertToString(std::wstring(szFriendlyName));
				CoTaskMemFree(szFriendlyName);

				hr = ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));
				if (FAILED(hr)) {
					SLogger::log("Failed to active camera object!");
					if (nullptr != pSource) pSource->Release();
					pSource = nullptr;
					continue;
				}
				hr = MFCreateSourceReaderFromMediaSource(pSource, nullptr, &pReader);
				if (FAILED(hr)) {
					SLogger::log("Failed to create reader from camera source object!");
					if (nullptr != pSource) pSource->Release();
					if (nullptr != pReader) pReader->Release();
					pSource = nullptr;
					pReader = nullptr;
					continue;
				}

				Camera* pDev = new Camera();
				pDev->ID = m_Cameras.size();
				pDev->Name = DeviceName;
				pDev->pCamera = new CameraDevice();
				try {
					pDev->pCamera->init(static_cast<void*>(pReader));
				}
				catch (const CrossForgeException& e) {
					SLogger::logException(e);
					printf("Something went wrong during initialization of a camera device. See log for details.");
				}
				catch (...) {
					printf("Something went wrong during initialization of a camera device. See log for details.");
				}
				m_Cameras.push_back(pDev);	

				if (nullptr != pSource) pSource->Release();
				pSource = nullptr;
			}
		}
		else {
			throw CForgeExcept("Failed to enumerate camera device sources!");
		}

		if (nullptr != pConfig) pConfig->Release();
		
	}//enumerateCameraDevices

	int32_t SMediaDeviceManager::cameraCount()const {
		return m_Cameras.size();
	}//cameraCount

	CameraDevice* SMediaDeviceManager::camera(int32_t ID) {
		if (0 < ID || ID >= m_Cameras.size()) throw IndexOutOfBoundsExcept("ID");
		return m_Cameras[ID]->pCamera;
	}//camera

}//name space

#endif