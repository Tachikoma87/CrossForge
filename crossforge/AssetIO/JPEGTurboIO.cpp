#include <turbojpeg.h>
#include <crossforge/AssetIO/File.h>
#include <crossforge/Utility/CForgeUtility.h>
#include "JPEGTurboIO.h"

namespace CForge {
	JPEGTurboIO::JPEGTurboIO() : I2DImageIO("JPEGTurboIO") {
		m_PluginName = "JPEGTurbo Image IO";
	}//Constructor

	JPEGTurboIO::~JPEGTurboIO() {

	}//Destructor

	void JPEGTurboIO::init() {

	}//initialize

	void JPEGTurboIO::clear() {

	}//clear

	void JPEGTurboIO::load(const std::string Filepath, T2DImage<uint8_t>* pImgData) {
		File F;
		F.begin(Filepath, "rb");
		if (!F.valid()) throw CForgeExcept("Unable to open file: " + Filepath);

		int64_t Filesize = F.size(Filepath);
		uint8_t* pCompressed = new uint8_t[Filesize];
		F.read(pCompressed, Filesize);
		F.end();

		load(pCompressed, Filesize, pImgData);

		if (nullptr != pCompressed) delete[] pCompressed;
		pCompressed = nullptr;
	}//load

	void JPEGTurboIO::load(const uint8_t* pBuffer, const uint32_t BufferLength, T2DImage<uint8_t>* pImgData) {
		uint32_t JpegSize = BufferLength; //!< _jpegSize from above
		uint8_t* pCompressedImage = const_cast<uint8_t*>(pBuffer); //!< _compressedImage from above

		int JpegSubsamp, Width, Height;
		tjhandle JpegDecompressor = tjInitDecompress();
		if (0 != tjDecompressHeader2(JpegDecompressor, pCompressedImage, JpegSize, &Width, &Height, &JpegSubsamp)) {
			throw CForgeExcept("Failed to read header from compressed jpeg: " + std::string(tjGetErrorStr2(JpegDecompressor)));
		}
		unsigned char* pDstBuffer = new uint8_t[Width * Height * 3]; //!< will contain the decompressed image
		if (0 != tjDecompress2(JpegDecompressor, pCompressedImage, JpegSize, pDstBuffer, Width, 0/*pitch*/, Height, TJPF_RGB, TJFLAG_FASTDCT)) {
			if (nullptr != pDstBuffer) delete[] pDstBuffer;
			pDstBuffer = nullptr;
			throw CForgeExcept("Failed to decompress jpeg data: " + std::string(tjGetErrorStr2(JpegDecompressor)));
		}
		tjDestroy(JpegDecompressor);

		pImgData->init(Width, Height, T2DImage<uint8_t>::COLORSPACE_RGB, pDstBuffer);

		if (nullptr != pDstBuffer) delete[] pDstBuffer;
		pDstBuffer = nullptr;
	}//load

	void JPEGTurboIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) {
		const int32_t Quality = 85;
		const int32_t ColorComponents = 3;
		long unsigned int JpegSize = 0;
		unsigned char* pCompressedImage = nullptr; //!< Memory is allocated by tjCompress2 if _jpegSize == 0

		tjhandle JpegCompressor = tjInitCompress();
		int32_t PixelFormat = pixelFormat(pImgData->colorSpace());

		tjCompress2(JpegCompressor, pImgData->data(), pImgData->width(), 0, pImgData->height(), PixelFormat,
			&pCompressedImage, &JpegSize, TJSAMP_444, Quality, TJFLAG_FASTDCT);
		tjDestroy(JpegCompressor);

		// write to file
		File F;
		F.begin(Filepath, "wb");
		if (F.valid()) {
			F.write(pCompressedImage, JpegSize);
		}
		else {
			tjFree(pCompressedImage);
			throw CForgeExcept("Unable to create file: " + Filepath);
		}
		F.end();

		//to free the memory allocated by TurboJPEG (either by tjAlloc(), 
		//or by the Compress/Decompress) after you are done working on it:
		tjFree(pCompressedImage);
	}//store

	bool JPEGTurboIO::accepted(const std::string Filepath, Operation Op) {
		bool Rval = false;
		std::string FPath = CForgeUtility::toLowerCase(Filepath);
		if (FPath.find(".jpg") != std::string::npos) Rval = true;
		if (FPath.find(".jpeg") != std::string::npos) Rval = true;
		return Rval;
	}//accepted

	void JPEGTurboIO::release(void) {
		delete this;
	}//release

	int32_t JPEGTurboIO::pixelFormat(const T2DImage<uint8_t>::ColorSpace CS) {
		int32_t Rval = TJPF_UNKNOWN;
		switch (CS) {
		case T2DImage<uint8_t>::COLORSPACE_GRAYSCALE: Rval = TJPF_GRAY; break;
		case T2DImage<uint8_t>::COLORSPACE_RGB: Rval = TJPF_RGB; break;
		case T2DImage<uint8_t>::COLORSPACE_RGBA: Rval = TJPF_RGBA; break;
		default: {
			throw CForgeExcept("Unknown color space specified!");
		}
		}//switch[Color Space]
		return Rval;
	}//pixelFormat

	T2DImage<uint8_t>::ColorSpace JPEGTurboIO::pixelFormat(int32_t CS) {
		T2DImage<uint8_t>::ColorSpace Rval;
		switch (CS) {
		case TJPF_RGB: Rval = T2DImage<uint8_t>::COLORSPACE_RGB; break;
		case TJPF_RGBA: Rval = T2DImage<uint8_t>::COLORSPACE_RGBA; break;
		case TJPF_GRAY: Rval = T2DImage<uint8_t>::COLORSPACE_GRAYSCALE; break;
		default: {
			throw CForgeExcept("Unsupported colors space specified!");
		}
		}
		return Rval;
	}//pixelFormat

}//name space