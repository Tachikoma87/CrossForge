#include "WebPImageIO.h"

#include <webp/encode.h>
#include <webp/decode.h>
#include "File.h"
#include "../Utility/CForgeUtility.h"

namespace CForge {

	WebPImageIO::WebPImageIO(void): I2DImageIO("WebPImageIO") {
		m_PluginName = "WebP Image IO";
	}//Constructor

	WebPImageIO::~WebPImageIO(void) {

	}//Destructor


	void WebPImageIO::init(void) {
		// nothing to do here
	}//initialize

	void WebPImageIO::clear(void) {
		// nothing to do here
	}//clear

	void WebPImageIO::load(const std::string Filepath, T2DImage<uint8_t>* pImgData) {
		if (!File::exists(Filepath)) throw CForgeExcept("File" + Filepath + " does not exist!");

		uint8_t* pBuffer = nullptr;
		
		File F;
		F.begin(Filepath, "rb");	
		int64_t Filesize = File::size(Filepath);
		if (Filesize == 0) throw CForgeExcept("File " + Filepath + " contains no data!");
		pBuffer = new uint8_t[Filesize];
		F.read(pBuffer, Filesize);
		F.end();

		load(pBuffer, Filesize, pImgData);

		if (nullptr != pBuffer) delete [] pBuffer;

	}//load

	void WebPImageIO::load(const uint8_t* pBuffer, const uint32_t BufferLength, T2DImage<uint8_t>* pImgData) {
		int32_t Width = 0;
		int32_t Height = 0;

		uint8_t* pDecoded = WebPDecodeRGBA(pBuffer, BufferLength, &Width, &Height);
		if (nullptr == pDecoded) throw CForgeExcept("Decoding WebP image failed");

		pImgData->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGBA, pDecoded);

		WebPFree(pDecoded);
		pDecoded = nullptr;

	}//load

	void WebPImageIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) {
		uint8_t* pOutput = nullptr;
		uint32_t Size = 0;

		if (pImgData->colorSpace() == T2DImage<uint8_t>::COLORSPACE_RGB) {
			Size = WebPEncodeRGB(pImgData->data(), pImgData->width(), pImgData->height(), pImgData->width() * 3, 90, &pOutput);
		}
		else if (pImgData->colorSpace() == T2DImage<uint8_t>::COLORSPACE_RGBA) {
			Size = WebPEncodeRGBA(pImgData->data(), pImgData->width(), pImgData->height(), pImgData->width() * 4, 90, &pOutput);
		}
		else {
			throw CForgeExcept("Image has invalid color space to be stored as webp!");
		}
		if (Size == 0) throw CForgeExcept("Failed to encode WebP image!");

		File F;
		F.begin(Filepath, "wb");
		if (!F.valid()) {
			WebPFree(pOutput);
			throw CForgeExcept("Unable to create file at " + Filepath);
		}
		F.write(pOutput, Size);
		F.end();

		WebPFree(pOutput);
	}//store

	bool WebPImageIO::accepted(const std::string Filepath, Operation Op) {	
		std::string Str = CForgeUtility::toLowerCase(Filepath);
		if (Str.find(".webp") != !std::string::npos) return true;
		return false;
	}//accepted

	void WebPImageIO::release(void) {
		delete this;
	}//release

}