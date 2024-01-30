#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include "StbImageIO.h"
#include "File.h"
#include "../Utility/CForgeUtility.h"

namespace CForge {

	StbImageIO::StbImageIO(void): I2DImageIO("StbImageIO") {
		m_PluginName = "stb Image IO";
	}//Constructor

	StbImageIO::~StbImageIO(void) {

	}//Destructor

	void StbImageIO::init(void) {
		
	}//initialize

	void StbImageIO::clear(void) {
		// nothing to do here
	}//clear

	void StbImageIO::load(const std::string Filepath, T2DImage<uint8_t>* pImgData) {
		if (!File::exists(Filepath)) throw CForgeExcept("File " + Filepath + " could not be found!");
		int32_t Width = 0;
		int32_t Height = 0;
		int32_t Components = 0;
		uint8_t* pData = stbi_load(Filepath.c_str(), &Width, &Height, &Components, 0);
		if (nullptr == pData) throw CForgeExcept("File " + Filepath + " could not be loaded!");

		T2DImage<uint8_t>::ColorSpace CS;
		switch (Components) {
		case 1: CS = T2DImage<uint8_t>::COLORSPACE_GRAYSCALE; break;
		case 3: CS = T2DImage<uint8_t>::COLORSPACE_RGB; break;
		case 4: CS = T2DImage<uint8_t>::COLORSPACE_RGBA; break;
		default: CS = T2DImage<uint8_t>::COLORSPACE_UNKNOWN; break;
		}

		if (CS == T2DImage<uint8_t>::COLORSPACE_UNKNOWN) {
			stbi_image_free(pData);
			throw CForgeExcept("Image " + Filepath + " has unknown format!");
		}

		pImgData->init(Width, Height, CS, pData);

		stbi_image_free(pData);
		pData = nullptr;
	}//load

	void StbImageIO::load(const uint8_t* pBuffer, const uint32_t BufferLength, T2DImage<uint8_t>* pImgData) {
		if (nullptr == pBuffer) throw NullpointerExcept("pBuffer");
		if (0 == BufferLength) throw CForgeExcept("Buffer with size 0 specified!");

		int32_t Width = 0;
		int32_t Height = 0;
		int32_t Components = 0;
		uint8_t* pData = stbi_load_from_memory(pBuffer, BufferLength, &Width, &Height, &Components, 0);

		if (nullptr == pData) throw CForgeExcept("Image data stream could not be decompressed!");

		T2DImage<uint8_t>::ColorSpace CS;
		switch (Components) {
		case 1: CS = T2DImage<uint8_t>::COLORSPACE_GRAYSCALE; break;
		case 3: CS = T2DImage<uint8_t>::COLORSPACE_RGB; break;
		case 4: CS = T2DImage<uint8_t>::COLORSPACE_RGBA; break;
		default: CS = T2DImage<uint8_t>::COLORSPACE_UNKNOWN; break;
		}

		if (CS == T2DImage<uint8_t>::COLORSPACE_UNKNOWN) {
			stbi_image_free(pData);
			throw CForgeExcept("Image has unknown format!");
		}

		pImgData->init(Width, Height, CS, pData);

		stbi_image_free(pData);
		pData = nullptr;
	}//load

	void StbImageIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) {
		if (pImgData->width() == 0 || pImgData->height() == 0) throw CForgeExcept("Image has invalid dimensions!");
		if (pImgData->data() == nullptr) throw CForgeExcept("Image contains no data!");

		std::string Str = CForgeUtility::toLowerCase(Filepath);

		int32_t Rval = 0;

		if (Str.find(".png") != std::string::npos) {
			Rval = stbi_write_png(Filepath.c_str(), pImgData->width(), pImgData->height(), pImgData->componentsPerPixel(), pImgData->data(), 0);
		}
		else if (Str.find(".bmp") != std::string::npos) {
			Rval = stbi_write_bmp(Filepath.c_str(), pImgData->width(), pImgData->height(), pImgData->componentsPerPixel(), pImgData->data());
		}
		else if (Str.find(".tga") != std::string::npos) {
			Rval = stbi_write_tga(Filepath.c_str(), pImgData->width(), pImgData->height(), pImgData->componentsPerPixel(), pImgData->data());
		}
		else if (Str.find(".jpg") != std::string::npos) {
			Rval = stbi_write_jpg(Filepath.c_str(), pImgData->width(), pImgData->height(), pImgData->componentsPerPixel(), pImgData->data(), 90);
		}
		else if (Str.find(".jpeg") != std::string::npos) {
			Rval = stbi_write_jpg(Filepath.c_str(), pImgData->width(), pImgData->height(), pImgData->componentsPerPixel(), pImgData->data(), 80);
		}

		if (0 == Rval) throw CForgeExcept("Something went wrong storing image " + Filepath);

	}//store

	bool StbImageIO::accepted(const std::string Filepath, Operation Op) {
		bool Rval = false;
		std::string S = CForgeUtility::toLowerCase(Filepath);

		if (Op == OP_LOAD) {
			if (S.find(".jpeg") != std::string::npos) Rval = true;
			if (S.find(".jpg") != std::string::npos) Rval = true;
			if (S.find(".png") != std::string::npos) Rval = true;
			if (S.find(".tga") != std::string::npos) Rval = true;
			if (S.find(".bmp") != std::string::npos) Rval = true;
			if (S.find(".psd") != std::string::npos) Rval = true;
			if (S.find(".pic") != std::string::npos) Rval = true;
		}
		else {
			if (S.find(".png") != std::string::npos) Rval = true;
			if (S.find(".tga") != std::string::npos) Rval = true;
			if (S.find(".bmp") != std::string::npos) Rval = true;
			if (S.find(".jpg") != std::string::npos) Rval = true;
			if (S.find(".jpeg") != std::string::npos) Rval = true;
		}	
		
		return Rval;
	}//accepted

	void StbImageIO::release(void) {
		delete this;
	}//release

}//name space