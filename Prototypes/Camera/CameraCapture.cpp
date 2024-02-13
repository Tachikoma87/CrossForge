#ifdef _WIN32

#include <crossforge/Utility/CForgeUtility.h>
#include <crossforge/AssetIO/SAssetIO.h>
#include "CameraCapture.h"

#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <Mferror.h>

#include <stb_image.h>



#pragma comment(lib, "dxva2.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


namespace CForge {
	CameraCapture::CameraCapture() {

	}//Constructor

	CameraCapture::~CameraCapture() {

	}//Destructor##

//	std::string getVideoFormatDescriptor(GUID G) {
//		std::string Rval = "unknown";
//
//		if (G == MFVideoFormat_Base) Rval = "Base";
//		if (G == MFVideoFormat_RGB32) Rval = "RGB32";
//		if (G == MFVideoFormat_ARGB32) Rval = "ARGB32";
//		if (G == MFVideoFormat_RGB24) Rval = "RGB24";
//		if (G == MFVideoFormat_RGB555) Rval = "RGB555";
//		if (G == MFVideoFormat_RGB565) Rval = "RGB565";
//		if (G == MFVideoFormat_RGB8) Rval = "RGB8";
//		if (G == MFVideoFormat_L8) Rval = "L8";
//		if (G == MFVideoFormat_L16) Rval = "L16";
//		if (G == MFVideoFormat_D16) Rval = "D16";
//		if (G == MFVideoFormat_AI44) Rval = "AI44";
//		if (G == MFVideoFormat_AYUV) Rval = "AYUV";
//		if (G == MFVideoFormat_YUY2) Rval = "YUY2";
//		if (G == MFVideoFormat_YVYU) Rval = "YUYU";
//		if (G == MFVideoFormat_YVU9) Rval = "YVU9";
//		if (G == MFVideoFormat_UYVY) Rval = "UYVY";
//		if (G == MFVideoFormat_NV11) Rval = "NV11";
//		if (G == MFVideoFormat_NV12) Rval = "NV12";
////		if (G == MFVideoFormat_NV21) Rval = "NV21";
//		if (G == MFVideoFormat_YV12) Rval = "YV12";
//		if (G == MFVideoFormat_I420) Rval = "I420";
//		if (G == MFVideoFormat_IYUV) Rval = "IYUV";
//		if (G == MFVideoFormat_Y210) Rval = "Y210";
//		if (G == MFVideoFormat_Y216) Rval = "Y216";
//		if (G == MFVideoFormat_Y410) Rval = "Y410";
//		if (G == MFVideoFormat_Y416) Rval = "Y416";
//		if (G == MFVideoFormat_Y41P) Rval = "Y41P";
//		if (G == MFVideoFormat_Y41T) Rval = "Y41T";
//		if (G == MFVideoFormat_Y42T) Rval = "Y42T";
//		if (G == MFVideoFormat_P210) Rval = "P210";
//		if (G == MFVideoFormat_P216) Rval = "P216";
//		if (G == MFVideoFormat_P010) Rval = "P010";
//		if (G == MFVideoFormat_P016) Rval = "P016";
//		if (G == MFVideoFormat_v210) Rval = "v210";
//		if (G == MFVideoFormat_v216) Rval = "v216";
//		if (G == MFVideoFormat_v410) Rval = "v410";
//		if (G == MFVideoFormat_MP43) Rval = "MP43";
//		if (G == MFVideoFormat_MP4S) Rval = "MP4S";
//		if (G == MFVideoFormat_M4S2) Rval = "M4S2";
//		if (G == MFVideoFormat_MP4V) Rval = "MP4V";
//		if (G == MFVideoFormat_WMV1) Rval = "WMV1";
//		if (G == MFVideoFormat_WMV2) Rval = "WMV2";
//		if (G == MFVideoFormat_WMV3) Rval = "WMV3";
//		if (G == MFVideoFormat_WVC1) Rval = "WVC1";
//		if (G == MFVideoFormat_MSS1) Rval = "MSS1";
//		if (G == MFVideoFormat_MSS2) Rval = "MSS2";
//		if (G == MFVideoFormat_MPG1) Rval = "MPG1";
//		if (G == MFVideoFormat_DVSL) Rval = "DVSL";
//		if (G == MFVideoFormat_DVSD) Rval = "DVSD";
//		if (G == MFVideoFormat_DVHD) Rval = "DVHD";
//		if (G == MFVideoFormat_DV25) Rval = "DV25";
//		if (G == MFVideoFormat_DV50) Rval = "DV50";
//		if (G == MFVideoFormat_DVH1) Rval = "DVH1";
//		if (G == MFVideoFormat_DVC) Rval = "DVC";
//		if (G == MFVideoFormat_H264) Rval = "H264";
//		if (G == MFVideoFormat_H265) Rval = "H265";
//		if (G == MFVideoFormat_MJPG) Rval = "MJPG";
//		if (G == MFVideoFormat_420O) Rval = "420O";
//		if (G == MFVideoFormat_HEVC) Rval = "HEVC";
//		if (G == MFVideoFormat_HEVC_ES) Rval = "HEVC_ES";
//		if (G == MFVideoFormat_VP80) Rval = "VP80";
//		if (G == MFVideoFormat_VP90) Rval = "VP90";
//		if (G == MFVideoFormat_ORAW) Rval = "ORAW";
//
//
//		return Rval;
//	}//

	void enumerateCaptureFormats(IMFMediaSource* pSource) {
		IMFPresentationDescriptor* pPD = nullptr;
		IMFStreamDescriptor* pSD = nullptr;
		IMFMediaTypeHandler* pHandler = nullptr;
		IMFMediaType* pType = nullptr;
		int32_t Selected = false;

		HRESULT hr = S_OK;
		hr = pSource->CreatePresentationDescriptor(&pPD);
		hr = pPD->GetStreamDescriptorByIndex(0, &Selected, &pSD);
		hr = pSD->GetMediaTypeHandler(&pHandler);

		DWORD cTypes = 0;
		hr = pHandler->GetMediaTypeCount(&cTypes);

		printf("\tFound %d formats:\n", cTypes);

		for (DWORD i = 0; i < cTypes; ++i) {
			hr = pHandler->GetMediaTypeByIndex(i, &pType);

			GUID G, Subtype;
			pHandler->GetMajorType(&G);
			// output media type
			
			uint32_t FrameWidth, FrameHeight, FPS, Stub, Bitrate;
			MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &FPS, &Stub);
			MFGetAttributeRatio(pType, MF_MT_FRAME_SIZE, &FrameWidth, &FrameHeight);
			pType->GetGUID(MF_MT_SUBTYPE, &Subtype);
			Bitrate = MFGetAttributeUINT32(pType, MF_MT_AVG_BITRATE, 0);
			//printf("\t\t %dx%d @ %d FPS - %d Kb/s (%s)\n", FrameWidth, FrameHeight, FPS, Bitrate/8000, getVideoFormatDescriptor(Subtype).c_str());

			pType->Release();
		}

		pPD->Release();
		pSD->Release();
		pHandler->Release();
	}//enumerateCaptureFormats

	HRESULT enumerateTypesForStream(IMFSourceReader* pReader, DWORD dwStreamIndex) {

		HRESULT hr = S_OK;
		DWORD dwMediaTypeIndex = 0;
		while (SUCCEEDED(hr)) {
			IMFMediaType* pType = nullptr;
			hr = pReader->GetNativeMediaType(dwStreamIndex, dwMediaTypeIndex, &pType);
			
			if (hr == MF_E_NO_MORE_TYPES) {
				hr = S_OK;
				break;
			}
			else if(SUCCEEDED(hr)){
				GUID Subtype;
				
				// output media type
				uint32_t FrameWidth, FrameHeight, FPS, Stub, Bitrate;
				MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &FPS, &Stub);
				MFGetAttributeRatio(pType, MF_MT_FRAME_SIZE, &FrameWidth, &FrameHeight);
				pType->GetGUID(MF_MT_SUBTYPE, &Subtype);
				Bitrate = MFGetAttributeUINT32(pType, MF_MT_AVG_BITRATE, 0);
				//printf("\t\t %dx%d @ %d FPS - %d (%s)\n", FrameWidth, FrameHeight, FPS, Stub, getVideoFormatDescriptor(Subtype).c_str());
				pType->Release();
			}
			dwMediaTypeIndex++;
		}
		return hr;
	}//enumerateTypesForStream

	void enumerateMediaTypes(IMFSourceReader* pReader) {
		HRESULT hr = S_OK;
		DWORD dwStreamIndex = 0;

		while (SUCCEEDED(hr)) {
			hr = enumerateTypesForStream(pReader, dwStreamIndex);
			if (hr == MF_E_INVALIDSTREAMNUMBER) {
				hr = S_OK;
				break;
			}
			dwStreamIndex++;
		}
	}

	HRESULT configureDecoder(IMFSourceReader* pReader, DWORD dwStreamIndex) {

		IMFMediaType* pNativeType = nullptr;
		IMFMediaType* pType = nullptr;
		HRESULT hr = S_OK;

		// find the native format of the stream
		hr = pReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pNativeType);
		if (FAILED(hr)) printf("Failed to get native media type\n");

		GUID MajorType, Subtype;
		// find the major type
		hr = pNativeType->GetGUID(MF_MT_MAJOR_TYPE, &MajorType);
		hr = pNativeType->GetGUID(MF_MT_SUBTYPE, &Subtype);

		// define the output type
		hr = MFCreateMediaType(&pType);
		//pType->SetGUID(MF_MT_MAJOR_TYPE, MajorType);
		pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG);

		MFSetAttributeRatio(pType, MF_MT_FRAME_SIZE, 1280, 720);
		//MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, 10, 1);

		hr = pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pType);

		switch (hr) {
		case S_OK: printf("Changed video stream\n"); break;
		case MF_E_INVALIDMEDIATYPE: printf("Media type was rejected!\n"); break;
		case MF_E_INVALIDREQUEST: printf("Request are pending!\n"); break;
		case MF_E_INVALIDSTREAMNUMBER: printf("Stream index parameter invalid!\n");
		case MF_E_TOPO_CODEC_NOT_FOUND: printf("Could not find a decoder for the native stream type!\n");
		default: printf("Something else gone wrong!\n");
		}


		pNativeType->Release();
		pType->Release();

		return hr;

	}//configureDecoder

	void processSample(IMFSourceReader* pReader) {
		HRESULT hr = S_OK;
		IMFSample* pSample = nullptr;
		size_t cSamples = 0;
		DWORD TotalLength;

		uint64_t Timestamp = CForgeUtility::timestamp();

		uint8_t* pBuffer = nullptr;
		uint8_t* pImageData = nullptr;
		DWORD BufferSize = 0;
		DWORD MaxSize = 0;

		int32_t Width, Height, Channels;

		IMFMediaBuffer *pMediaBuffer = nullptr;

		uint64_t FPSTimer = CForgeUtility::timestamp();
		int32_t FPSCounter = 0;
		

		bool quit = false;
		int32_t c = 0;
		while (CForgeUtility::timestamp() - Timestamp < 5000) {
			DWORD StreamIndex, Flags;
			LONGLONG llTimeStamp;

			hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
				0,
				&StreamIndex, &Flags, &llTimeStamp,
				&pSample);

			if (Flags & MF_SOURCE_READERF_ENDOFSTREAM) printf("End of stream!\n");
			if (Flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED) printf("Native media changed!\n");

			if (FAILED(hr)) printf("Failed to read sample!\n");

			else if (nullptr != pSample) {
				pSample->GetTotalLength(&TotalLength);
				pSample->GetBufferByIndex(0, &pMediaBuffer);

				pMediaBuffer->Lock(&pBuffer, &MaxSize, &BufferSize);
				T2DImage<uint8_t> Img;
				AssetIO::load(pBuffer, uint32_t(BufferSize), &Img);
				pMediaBuffer->Unlock();
				pMediaBuffer->Release();
				pMediaBuffer = nullptr;

				std::string Filename = "Assets/Webcam/" + std::to_string(c++) + ".jpg";
				AssetIO::store(Filename, &Img);
	
				FPSCounter++;
				if (CForgeUtility::timestamp() - FPSTimer > 1000) {
					FPSTimer = CForgeUtility::timestamp();
					printf("FPS: %d\n", FPSCounter);
					FPSCounter = 0;
				}
				pSample->Release();
				pSample = nullptr;
			}
			else {
				printf("Something went wrong\n");
			}
			//Sleep(100);
		}

		if (nullptr != pSample) pSample->Release();
		printf("Finished video capturing\n");
	}//processSample

	
	HRESULT createVideoCaptureDevice(IMFMediaSource** ppSource) {
		*ppSource = nullptr;

		uint32_t Count = 0;

		IMFAttributes* pConfig = nullptr;
		IMFActivate** ppDevices = nullptr;
		IMFMediaSource* pSource = nullptr;

		// Create an attribute store to hold the search criteria
		HRESULT hr = MFCreateAttributes(&pConfig, 1);

		// Request video capture devices
		if (SUCCEEDED(hr)) {
			hr = pConfig->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
		}

		if (SUCCEEDED(hr)) {
			hr = MFEnumDeviceSources(pConfig, &ppDevices, &Count);
		}

		// Create a media source for the first device in the list
		if (SUCCEEDED(hr)) {
			for (uint32_t i = 0; i < Count; ++i) {
				hr = ppDevices[i]->ActivateObject(IID_PPV_ARGS(&pSource));
				if (SUCCEEDED(hr)) {
					WCHAR* szFriendlyName = nullptr;
					uint32_t cchName;
					hr = ppDevices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &szFriendlyName, &cchName);

					if (SUCCEEDED(hr)) {
						wprintf(L"\tVideo device name: %s\n", szFriendlyName);
						
					}
					//enumerateCaptureFormats(pSource);

					IMFSourceReader* pReader;
				
					MFCreateSourceReaderFromMediaSource(pSource, nullptr, &pReader);
					enumerateMediaTypes(pReader);
					configureDecoder(pReader, 0);

					processSample(pReader);

					//stbi_load_from_memory()

					pReader->Release();

					
					CoTaskMemFree(szFriendlyName);

					
				}
			} 
			
		}

		for (uint32_t i = 0; i < Count; ++i) {
			ppDevices[i]->ShutdownObject();
			ppDevices[i]->Release();
		}
		CoTaskMemFree(ppDevices);

		return S_OK;
	}//createVideoCaptureDevice

	void CameraCapture::init() {
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if(S_OK != MFStartup(MF_VERSION)) throw CForgeExcept("Initializing media foundation failed\n");
		printf("Starting camera initialization ...\n");
		IMFMediaSource* pSource = nullptr;
		createVideoCaptureDevice(&pSource);
	}//initialize

	void CameraCapture::clear() {

	}//clear

	void CameraCapture::release() {
		delete this;
	}//release
}

#endif