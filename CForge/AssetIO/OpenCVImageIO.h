/*****************************************************************************\
*                                                                           *
* File(s): OpenCVImageIO.h and OpenCVImageIO.cpp                             *
*                                                                           *
* Content:    *
*          .                                         *
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
#ifndef __CFORGE_OPENCVIMAGEIO_H__
#define __CFORGE_OPENCVIMAGEIO_H__
#ifdef USE_OPENCV

#include <opencv2/core.hpp>
#include "I2DImageIO.h"

namespace CForge {

	/**
	* \brief Image import/export plugin that employs OpenCV
	*
	* \todo Implement all conversion methods
	* \todo Full documentation
	*/
	class OpenCVImageIO : public I2DImageIO {
	public:
		OpenCVImageIO(void);
		~OpenCVImageIO(void);

		void init(void);
		void clear();
		
		void load(const std::string Filepath, T2DImage<uint8_t>* pImgData);
		void store(const std::string Filepath, const T2DImage<uint8_t>* pImgData);
		bool accepted(const std::string Filepath, Operation Op);

		void release(void);
	protected:
		void convertCVImageToCFImageGrayscale(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg);
		void convertCVImageToCFImageRGB(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg);
		void convertCVImageToCFImageRBBA(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg);

		void convertCFImageToCVImageGrayscale(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg);
		void convertCFImageToCVImageRGB(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg);
		void convertCFImageToCVImageRGBA(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg);

	};//OpenCVImageIO

}//name space

#endif
#endif 