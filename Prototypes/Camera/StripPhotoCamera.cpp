#include "StripPhotoCamera.h"
#include <crossforge/AssetIO/SAssetIO.h>

#ifdef INCLUDE_OPENCV

using namespace cv;

namespace CForge {

	StripPhotoCamera::StripPhotoCamera(void) {

	}//Constructor

	StripPhotoCamera::~StripPhotoCamera(void) {

	}//Destructor


	void StripPhotoCamera::init(void) {
		clear();

		//m_StripPhoto.init(1.5f, 1000);

		m_Camera.open(0, cv::VideoCaptureAPIs::CAP_DSHOW);
		if (!m_Camera.isOpened()) {
			throw CForgeExcept("Failed to open camera device 0");
		}
		else {
			m_Camera.set(cv::VideoCaptureProperties::CAP_PROP_FPS, 30);
			m_Camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1280);
			m_Camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 720);
			
		}

		/*if (!m_Camera.get(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB)) {
			throw CForgeExcept("Camera does not support convert RGB");
		}*/

		//m_LastFPSPrint = Utilities::timestampMilliseconds();

		printf("w x h : %f x %f @ %f FPS\n", 
			m_Camera.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH), 
			m_Camera.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT),
			m_Camera.get(cv::VideoCaptureProperties::CAP_PROP_FPS));

	}//initialize

	void StripPhotoCamera::clear(void) {

	}//clear

	void StripPhotoCamera::update(void) {
		if (!m_Camera.read(m_ImageFrame)) return;

		/*T2DImage<uint8_t> TestImg;
		convertCVImageToCFImageRGB(&m_ImageFrame, &TestImg);
		AssetIO::store("Assets/CameraImg.jpg", &TestImg);*/

		// extract line
		Eigen::Matrix<Eigen::Matrix<uint8_t, 3, 1>, 2, 2> Line;
		
		cv::Mat3b CVLine(m_ImageFrame.rows, 51);

		uint32_t Mid = m_ImageFrame.cols / 2;

		Line.resize(m_ImageFrame.rows, 51);
		uint8_t Col = 0;

		for (int x = Mid - 25; x < Mid + 25; x++) {
			for (int y = 0; y < m_ImageFrame.rows; ++y) {
				cv::Vec3b PixelData = m_ImageFrame.at<cv::Vec3b>(y, x);
				Line.coeffRef(y, Col) = Eigen::Matrix<uint8_t,3,1>(PixelData[0], PixelData[1], PixelData[2]);

				CVLine.at<cv::Vec3b>(y, Col) = PixelData;
			}
			Col++;
		}//for[columns]

		//m_LastFrame = CVLine;
		convertCVImageToCFImageRGB(&CVLine, &m_LastFrame);

		// count FPS
		/*m_FPSCounter++;
		if (Utilities::timestampMilliseconds() - m_LastFPSPrint > 1000) {
			printf("FPS: %d\n", m_FPSCounter);
			m_LastFPSPrint = Utilities::timestampMilliseconds();
			m_FPSCounter = 0;
		}*/

		
	}//update

	void StripPhotoCamera::lastFrame(T2DImage<uint8_t> *pImg) {
		pImg->init(m_LastFrame.width(), m_LastFrame.height(), m_LastFrame.colorSpace(), m_LastFrame.data());
	}//lastFrame

	void StripPhotoCamera::convertCVImageToCFImageRGB(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg) {
		// convert to Image2D
		uint8_t* pImgData = new uint8_t[pCVImg->rows * pCVImg->cols * 3];

		for (uint32_t y = 0; y < pCVImg->rows; ++y) {
			for (uint32_t x = 0; x < pCVImg->cols; ++x) {
				const Vec3b Pixel = pCVImg->at<Vec3b>(y, x);
				const uint32_t Index = ((pCVImg->rows - y - 1) * pCVImg->cols + x) * 3;
				pImgData[Index + 0] = Pixel[2];
				pImgData[Index + 1] = Pixel[1];
				pImgData[Index + 2] = Pixel[0];
			}//for[columns]
		}//for[rows]

		// initialize image
		pImg->init(pCVImg->cols, pCVImg->rows, T2DImage<uint8_t>::COLORSPACE_RGB, pImgData);

		// clean up
		/*delete[] pImgData;
		pImgData = nullptr;*/

	}//convertCVImageToCFImageRGB

}//name-space

#endif