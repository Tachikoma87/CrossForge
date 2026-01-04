#ifdef _WIN32

#include <crossforge/Core/SLogger.h>
#include <crossforge/Math/CForgeMath.h>
#include <crossforge/AssetIO/SAssetIO.h>

#include "CameraDevice.h"

#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <Mferror.h>

#include "FFMPEG.h"


namespace CForge {
	std::string getVideoFormatDescriptor(GUID G) {
		std::string Rval = "unknown";

		if (G == MFVideoFormat_Base) Rval = "Base";
		if (G == MFVideoFormat_RGB32) Rval = "RGB32";
		if (G == MFVideoFormat_ARGB32) Rval = "ARGB32";
		if (G == MFVideoFormat_RGB24) Rval = "RGB24";
		if (G == MFVideoFormat_RGB555) Rval = "RGB555";
		if (G == MFVideoFormat_RGB565) Rval = "RGB565";
		if (G == MFVideoFormat_RGB8) Rval = "RGB8";
		if (G == MFVideoFormat_L8) Rval = "L8";
		if (G == MFVideoFormat_L16) Rval = "L16";
		if (G == MFVideoFormat_D16) Rval = "D16";
		if (G == MFVideoFormat_AI44) Rval = "AI44";
		if (G == MFVideoFormat_AYUV) Rval = "AYUV";
		if (G == MFVideoFormat_YUY2) Rval = "YUY2";
		if (G == MFVideoFormat_YVYU) Rval = "YUYU";
		if (G == MFVideoFormat_YVU9) Rval = "YVU9";
		if (G == MFVideoFormat_UYVY) Rval = "UYVY";
		if (G == MFVideoFormat_NV11) Rval = "NV11";
		if (G == MFVideoFormat_NV12) Rval = "NV12";
		//		if (G == MFVideoFormat_NV21) Rval = "NV21"; // not available in windows 10
		if (G == MFVideoFormat_YV12) Rval = "YV12";
		if (G == MFVideoFormat_I420) Rval = "I420";
		if (G == MFVideoFormat_IYUV) Rval = "IYUV";
		if (G == MFVideoFormat_Y210) Rval = "Y210";
		if (G == MFVideoFormat_Y216) Rval = "Y216";
		if (G == MFVideoFormat_Y410) Rval = "Y410";
		if (G == MFVideoFormat_Y416) Rval = "Y416";
		if (G == MFVideoFormat_Y41P) Rval = "Y41P";
		if (G == MFVideoFormat_Y41T) Rval = "Y41T";
		if (G == MFVideoFormat_Y42T) Rval = "Y42T";
		if (G == MFVideoFormat_P210) Rval = "P210";
		if (G == MFVideoFormat_P216) Rval = "P216";
		if (G == MFVideoFormat_P010) Rval = "P010";
		if (G == MFVideoFormat_P016) Rval = "P016";
		if (G == MFVideoFormat_v210) Rval = "v210";
		if (G == MFVideoFormat_v216) Rval = "v216";
		if (G == MFVideoFormat_v410) Rval = "v410";
		if (G == MFVideoFormat_MP43) Rval = "MP43";
		if (G == MFVideoFormat_MP4S) Rval = "MP4S";
		if (G == MFVideoFormat_M4S2) Rval = "M4S2";
		if (G == MFVideoFormat_MP4V) Rval = "MP4V";
		if (G == MFVideoFormat_WMV1) Rval = "WMV1";
		if (G == MFVideoFormat_WMV2) Rval = "WMV2";
		if (G == MFVideoFormat_WMV3) Rval = "WMV3";
		if (G == MFVideoFormat_WVC1) Rval = "WVC1";
		if (G == MFVideoFormat_MSS1) Rval = "MSS1";
		if (G == MFVideoFormat_MSS2) Rval = "MSS2";
		if (G == MFVideoFormat_MPG1) Rval = "MPG1";
		if (G == MFVideoFormat_DVSL) Rval = "DVSL";
		if (G == MFVideoFormat_DVSD) Rval = "DVSD";
		if (G == MFVideoFormat_DVHD) Rval = "DVHD";
		if (G == MFVideoFormat_DV25) Rval = "DV25";
		if (G == MFVideoFormat_DV50) Rval = "DV50";
		if (G == MFVideoFormat_DVH1) Rval = "DVH1";
		if (G == MFVideoFormat_DVC) Rval = "DVC";
		if (G == MFVideoFormat_H264) Rval = "H264";
		if (G == MFVideoFormat_H265) Rval = "H265";
		if (G == MFVideoFormat_MJPG) Rval = "MJPG";
		if (G == MFVideoFormat_420O) Rval = "420O";
		if (G == MFVideoFormat_HEVC) Rval = "HEVC";
		if (G == MFVideoFormat_HEVC_ES) Rval = "HEVC_ES";
		if (G == MFVideoFormat_VP80) Rval = "VP80";
		if (G == MFVideoFormat_VP90) Rval = "VP90";
		if (G == MFVideoFormat_ORAW) Rval = "ORAW";

		return Rval;
	}//


	CameraDevice::CameraDevice() {
		m_pDeviceHandle = nullptr;
	}//Constructor

	CameraDevice::~CameraDevice() {
		clear();
	}//Destructor

	void CameraDevice::init(void* pDeviceHandle) {
		if (nullptr == pDeviceHandle) throw NullpointerExcept("pDeviceHandle");

		m_pDeviceHandle = pDeviceHandle;
		enumerateCaptureFormats();

		// debug
		printf("Found %d formats for camera device\n", int32_t(m_CaptureFormats.size()));
		for (auto i : m_CaptureFormats) {
			std::string Message = "\t" + std::to_string(i.FrameSize.x()) + "x" + std::to_string(i.FrameSize.y()) + " - ";
			for (auto k : i.FPS) Message += std::to_string(k) + "/";
			Message += " FPS (" + i.DataFormat + ")";

			printf("%s\n", Message.c_str());	
		}
		printf("\n\n");

	}//initialize

	void CameraDevice::clear() {
		if (nullptr != m_pDeviceHandle) static_cast<IMFSourceReader*>(m_pDeviceHandle)->Release();
		m_pDeviceHandle = nullptr;
		m_ActiveCaptureFormat = 0;
	}//clear

	void CameraDevice::release() {
		delete this;
	}//release

	void CameraDevice::enumerateCaptureFormats() {
		if (nullptr == m_pDeviceHandle) throw NullpointerExcept("m_pDeviceHandle");

		IMFSourceReader* pReader = static_cast<IMFSourceReader*>(m_pDeviceHandle);

		std::list<CaptureFormat> FormatsList;

		// enumerate all formats for all streams
		DWORD StreamIndex = 0;
		DWORD CaptureFormatIndex = 0;
		IMFMediaType* pType = nullptr;
		HRESULT hr = S_OK;

		// outer loop (streams)
		while (SUCCEEDED(hr)) {
			// inner loop (capture formats for each stream)
			CaptureFormatIndex = 0;
			do {
				hr = pReader->GetNativeMediaType(StreamIndex, CaptureFormatIndex, &pType);
				if (SUCCEEDED(hr)) {
					CaptureFormat Format;
					Format.StreamIndex = StreamIndex;
					uint32_t FrameWidth, FrameHeight, Stub, FPS;
					MFGetAttributeRatio(pType, MF_MT_FRAME_SIZE, &FrameWidth, &FrameHeight);
					Format.FrameSize = Eigen::Vector2i(FrameWidth, FrameHeight);
					MFGetAttributeRatio(pType, MF_MT_FRAME_RATE, &FPS, &Stub);

					GUID Subtype;
					pType->GetGUID(MF_MT_SUBTYPE, &Subtype);
					Format.DataFormat = getVideoFormatDescriptor(Subtype);
					Format.FPS.push_back(FPS);

					//sort into list
					bool Inserted = false;
					for (auto i = FormatsList.begin(); i != FormatsList.end(); i++) {
						// first criterion is frame width and height
						if (i->FrameSize.x() < Format.FrameSize.x()) continue;
						if (i->FrameSize.y() < Format.FrameSize.y()) continue;
						if (i->FrameSize.x() == Format.FrameSize.x() && i->FrameSize.y() == Format.FrameSize.y()) {
							// same frame size
							// does same format already exist?
							for (auto &k : FormatsList) {
								if (k.FrameSize.x() != Format.FrameSize.x() || k.FrameSize.y() != Format.FrameSize.y()) continue;
								if (k.DataFormat.compare(Format.DataFormat) == 0) {
									k.FPS.push_back(FPS);
									Inserted = true;
									break;
								}
							}
							if (!Inserted) FormatsList.insert(i, Format);
							Inserted = true;
							break;
						}
						if (!Inserted) FormatsList.insert(i, Format);
						Inserted = true;
						break;
					}//for[formats list]

					// not suitable place found?
					if (!Inserted) {
						FormatsList.push_back(Format);
					}

					if (nullptr != pType) pType->Release();
					pType = nullptr;
				}
				CaptureFormatIndex++;
			} while (SUCCEEDED(hr));
			StreamIndex++;
		};

		// copy sorted list to storage
		for (auto &i : FormatsList) {
			// sort FPS
			std::list<uint32_t> FPSList;
			for (auto k : i.FPS) FPSList.push_back(k);
			FPSList.sort();
			i.FPS.clear();
			for (auto k : FPSList) i.FPS.push_back(k);

			m_CaptureFormats.push_back(i);
		}

	}//enumerateCaptureFormats


	void CameraDevice::changeCaptureFormat(int32_t FormatID, int32_t FPS) {
		if (nullptr == m_pDeviceHandle) throw NullpointerExcept("m_pDeviceHandle");
		if (0 > FormatID || FormatID >= m_CaptureFormats.size()) throw IndexOutOfBoundsExcept("FormatID");

		IMFSourceReader* pReader = static_cast<IMFSourceReader*>(m_pDeviceHandle);
		IMFMediaType* pType = nullptr;
		HRESULT hr = S_OK;

		CaptureFormat F = m_CaptureFormats[FormatID];

		// define the output type
		hr = MFCreateMediaType(&pType);
		pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		if (F.DataFormat.compare("MJPG") == 0) pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG);
		else if (F.DataFormat.compare("NV12") == 0) pType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
		else throw CForgeExcept("Invalid data format defined for video stream!");

		MFSetAttributeRatio(pType, MF_MT_FRAME_SIZE, F.FrameSize.x(), F.FrameSize.y());
		// if FPS < 0 use default value (max fps I suppose)
		if(FPS > 0) MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, FPS, 1);

		hr = pReader->SetCurrentMediaType(F.StreamIndex, nullptr, pType);
		pType->Release();

		if (FAILED(hr)) {
			std::string Error = "";
			switch (hr) {
			case MF_E_INVALIDMEDIATYPE: Error = "Media type was rejected!"; break;
			case MF_E_INVALIDREQUEST: Error = "Request are pending!"; break;
			case MF_E_INVALIDSTREAMNUMBER: Error = "Stream index parameter invalid!"; break;
			case MF_E_TOPO_CODEC_NOT_FOUND: Error = "Could not find a decoder for the native stream type!"; break;
			default: {
				Error = "Unknown error occurred"; 
				break;
			}
			}//switch[hr]

			throw CForgeExcept("Failed to change camera device's capture format: " + Error);
		}

		m_ActiveCaptureFormat = FormatID;
	}//changeCaptureFormat

	uint32_t CameraDevice::captureFormatCount()const {
		return m_CaptureFormats.size();
	}//captureFormatCount

	CameraDevice::CaptureFormat CameraDevice::captureFormat(int32_t ID)const {
		if (0 > ID || ID >= m_CaptureFormats.size()) throw IndexOutOfBoundsExcept("ID");
		return m_CaptureFormats[ID];
	}//captureFormat

	void CameraDevice::findOptimalCaptureFormats(int32_t Width, int32_t Height, std::vector<int32_t>* pFormatIDs) {
		if (nullptr == pFormatIDs) throw NullpointerExcept("pFormatIDs");
		pFormatIDs->clear();

		// try to find formats that exactly match the specified values
		for (uint32_t i = 0; i < m_CaptureFormats.size(); ++i) {
			CaptureFormat F = m_CaptureFormats[i];
			if (F.FrameSize.x() == Width && F.FrameSize.y() == Height) pFormatIDs->push_back(i);
		}
		if (pFormatIDs->size() > 0) return;

		// try to find formats where at least one value matches
		for (uint32_t i = 0; i < m_CaptureFormats.size(); ++i) {
			CaptureFormat F = m_CaptureFormats[i];
			if (F.FrameSize.x() == Width || F.FrameSize.y() == Height) pFormatIDs->push_back(i);
		}
		if (pFormatIDs->size() > 0) return;

		int32_t BestCandidate = 0;
		for (uint32_t i = 0; i < m_CaptureFormats.size(); ++i) {
			int32_t D1 = std::abs(Width - m_CaptureFormats[i].FrameSize.x());
			int32_t D2 = std::abs(Width - m_CaptureFormats[BestCandidate].FrameSize.x());
			if (D1 < D2) BestCandidate = i;
		}
		findOptimalCaptureFormats(m_CaptureFormats[BestCandidate].FrameSize.x(), 0, pFormatIDs);

	}//findOptimalCaptureFormats

	void CameraDevice::retrieveImage(T2DImage<uint8_t>* pImg) {
		if (nullptr == pImg) throw NullpointerExcept("pImg");
		if (nullptr == m_pDeviceHandle) throw NullpointerExcept("m_pDeviceHandle");

		IMFSourceReader* pReader = static_cast<IMFSourceReader*>(m_pDeviceHandle);

		IMFSample* pSample = nullptr;
		IMFMediaBuffer* pMediaBuffer = nullptr;

		size_t cSamples = 0;
		DWORD StreamIndex, Flags;
		LONGLONG llTimeStamp;

		uint8_t* pBuffer = nullptr;
		DWORD MaxSize = 0;
		DWORD BufferSize = 0;

		HRESULT hr = pReader->ReadSample(m_CaptureFormats[m_ActiveCaptureFormat].StreamIndex, 0, &StreamIndex, &Flags, &llTimeStamp, &pSample);
		if (FAILED(hr)) throw CForgeExcept("Failed to read image from video stream!");
		if (nullptr != pSample) {
			pSample->GetBufferByIndex(0, &pMediaBuffer);

			pMediaBuffer->Lock(&pBuffer, &MaxSize, &BufferSize);
			const CaptureFormat CF = m_CaptureFormats[m_ActiveCaptureFormat];
			if(CF.DataFormat.compare("MJPG") == 0){
				//printf("received mjpeg image\n");
				AssetIO::load(pBuffer, uint32_t(BufferSize), pImg);
			}
			else if (CF.DataFormat.compare("NV12") == 0) {
				//printf("received nv12 image\n");
				FFMPEG FP;
				FP.convertNV12(pBuffer, BufferSize, CF.FrameSize.x(), CF.FrameSize.y(), pImg);

			}

			pMediaBuffer->Unlock();

			if (nullptr != pMediaBuffer) pMediaBuffer->Release();
			if (nullptr != pSample) pSample->Release();
			pSample = nullptr;
			pMediaBuffer = nullptr;

			
		}

		changeCaptureFormat(m_ActiveCaptureFormat);
	}//retrieveImage

}//name space

#endif