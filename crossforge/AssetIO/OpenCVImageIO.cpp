#include "OpenCVImageIO.h"
#include "../AssetIO/File.h"

#ifdef USE_OPENCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace cv;

namespace CForge {

	OpenCVImageIO::OpenCVImageIO(void): I2DImageIO("OpenCVImageIO") {

	}//Constructor

	OpenCVImageIO::~OpenCVImageIO(void) {

	}//Destructor


	void OpenCVImageIO::init(void) {

	}//initialize

	void OpenCVImageIO::clear() {

	}//clear

	void OpenCVImageIO::load(const std::string Filepath, T2DImage<uint8_t>* pImgData) {
		if (!File::exists(Filepath)) throw CForgeExcept("File " + Filepath + " could not be found!");

		Mat Img = imread(Filepath);

		if (Img.empty()) throw CForgeExcept("Could not load image " + Filepath);

		if (Img.type() == CV_8UC3) {
			// BGR color image
			convertCVImageToCFImageRGB(&Img, pImgData);
		}
		else if (Img.type() == CV_8UC4) {
			// BGRA image
			convertCVImageToCFImageRBBA(&Img, pImgData);
		}
		else if(Img.type() == CV_8UC1){
			// Grayscale image
			convertCVImageToCFImageGrayscale(&Img, pImgData);
		}
		else {
			throw CForgeExcept("Image " + Filepath + " has unknown color format.");
		}

	}//load

	void OpenCVImageIO::store(const std::string Filepath, const T2DImage<uint8_t>* pImgData) {
		if (nullptr == pImgData) throw NullpointerExcept("pImgData");
		if (Filepath.empty()) throw CForgeExcept("Empty filepath specified!");

		cv::Mat CVImg;

		switch (pImgData->colorSpace()) {
			case T2DImage<uint8_t>::COLORSPACE_GRAYSCALE:{
				convertCFImageToCVImageGrayscale(&CVImg, pImgData);
			}break;
			case T2DImage<uint8_t>::COLORSPACE_RGB: {
				convertCFImageToCVImageRGB(&CVImg, pImgData);
			}break;
			case T2DImage<uint8_t>::COLORSPACE_RGBA:{
				convertCFImageToCVImageRGBA(&CVImg, pImgData);
			}break;
			default: throw CForgeExcept("Invalid color space. Can not store this image.");
		}//switch[color space]

		imwrite(Filepath, CVImg);

	}//store

	bool OpenCVImageIO::accepted(const std::string Filepath, Operation Op) {
		return true;
	}//accepted


	void OpenCVImageIO::convertCVImageToCFImageGrayscale(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg) {

	}//convertCVImageToCFImageGrayscale

	void OpenCVImageIO::convertCVImageToCFImageRGB(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg) {
		// convert to Image2D
		uint8_t* pImgData = new uint8_t[pCVImg->rows * pCVImg->cols * 3];

		for (uint32_t y = 0; y < pCVImg->rows; ++y) {
			for (uint32_t x = 0; x < pCVImg->cols; ++x) {
				const Vec3b Pixel = pCVImg->at<Vec3b>(y, x);
				const uint32_t Index =  ( (pCVImg->rows - y - 1) * pCVImg->cols + x) * 3;
				pImgData[Index + 0] = Pixel[2];
				pImgData[Index + 1] = Pixel[1];
				pImgData[Index + 2] = Pixel[0];
			}//for[columns]
		}//for[rows]

		// initialize image
		pImg->init(pCVImg->cols, pCVImg->rows, T2DImage<uint8_t>::COLORSPACE_RGB, pImgData);

		// clean up
		delete[] pImgData;
		pImgData = nullptr;

	}//convertCVImageToCFImageRGB

	void OpenCVImageIO::convertCVImageToCFImageRBBA(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg) {

	}//convertCVImageToCFImageRGBA

	void OpenCVImageIO::convertCFImageToCVImageGrayscale(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg) {
		if (nullptr == pCVImg) throw NullpointerExcept("pCVImg");
		if (nullptr == pImg) throw NullpointerExcept("pImg");

		(*pCVImg) = cv::Mat(pImg->height(), pImg->width(), CV_8UC1, Scalar(0));

		// copy data
		for (uint32_t y = 0; y < pImg->height(); ++y) {
			for (uint32_t x = 0; x < pImg->width(); ++x) {
				const uint8_t* pPixel = pImg->pixel(x, pImg->height() - y - 1);
				pCVImg->at<uint8_t>(y, x) = pPixel[0];
			}
		}
	}//convertCFImageToCVImageGrayscale

	void OpenCVImageIO::convertCFImageToCVImageRGB(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg) {
		if (nullptr == pCVImg) throw NullpointerExcept("pCVImg");
		if (nullptr == pImg) throw NullpointerExcept("pImg");

		// create appropriate image
		(*pCVImg) = cv::Mat(pImg->height(), pImg->width(), CV_8UC3, Scalar(0, 0, 0));

		// copy data, convert form RGB to BGR
		for (uint32_t y = 0; y < pImg->height(); ++y) {
			for (uint32_t x = 0; x < pImg->width(); x++) {
				const uint8_t* pPixel = pImg->pixel(x, pImg->height() - y - 1);

				pCVImg->at<Vec3b>(y, x)[0] = pPixel[2];
				pCVImg->at<Vec3b>(y, x)[1] = pPixel[1];
				pCVImg->at<Vec3b>(y, x)[2] = pPixel[0];

			}//for[columns]
		}//for[rows]

	}//convertCFImageToCVImageRBB

	void OpenCVImageIO::convertCFImageToCVImageRGBA(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg) {

	}//convertCFImageToCVImageRGBA

	void OpenCVImageIO::release(void) {
		delete this;
	}//release
}//name space

#endif