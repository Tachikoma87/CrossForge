#include "WebPImageIO.h"

#include <webp/encode.h>
#include <webp/decode.h>
#include <CForge/AssetIO/File.h>

namespace CForge {

	WebPImageIO::WebPImageIO(void) {

	}//Constructor

	WebPImageIO::~WebPImageIO(void) {

	}//Destructor

	void WebPImageIO::load(const std::string Filepath, T2DImage<uint8_t>* pImgData) {
		if (!File::exists(Filepath)) throw CForgeExcept("File" + Filepath + " does not exist!");

		uint8_t* pBuffer = nullptr;
		int32_t Width = 0;
		int32_t Height = 0;

		File F;
		F.begin(Filepath, "rb");	
		int64_t Filesize = File::size(Filepath);
		pBuffer = new uint8_t[Filesize];
		F.read(pBuffer, Filesize);
		F.end();

		uint8_t* pDecoded = WebPDecodeRGBA(pBuffer, Filesize, &Width, &Height);
		if (nullptr != pBuffer) delete[] pBuffer;

		if (nullptr == pDecoded) throw CForgeExcept("Decoding WebP image failed " + Filepath);

		pImgData->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGBA, pDecoded);
		pImgData->flipRows();

		WebPFree(pDecoded);
		pDecoded = nullptr;

	}//load

	void WebPImageIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) {
		uint8_t* pOutput = nullptr;

		T2DImage<uint8_t> Img;
		Img.init(pImgData->width(), pImgData->height(), pImgData->colorSpace(), pImgData->data());
		Img.flipRows();


		uint32_t Size = 0;
		if (Img.colorSpace() == T2DImage<uint8_t>::COLORSPACE_RGB) {
			Size = WebPEncodeRGB(Img.data(), Img.width(), Img.height(), Img.width() * 3, 90, &pOutput);
		}
		else if (Img.colorSpace() == T2DImage<uint8_t>::COLORSPACE_RGBA) {
			Size = WebPEncodeRGBA(Img.data(), Img.width(), Img.height(), Img.width() * 4, 90, &pOutput);
		}
		else {
			throw CForgeExcept("Image has invalid color space to be stored as webp!");
		}
		if (Size == 0) throw CForgeExcept("Failed to encode WebP image!");

		File F;
		F.begin(Filepath, "wb");
		F.write(pOutput, Size);
		F.end();

		WebPFree(pOutput);
	}//store

	//bool accepted(const std::string Filepath, Operation Op);

	void WebPImageIO::release(void) {
		delete this;
	}//release

}