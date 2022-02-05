#include "StripPhotoCamera.h"


namespace CForge {

	StripPhotoCamera::StripPhotoCamera(void) {

	}//Constructor

	StripPhotoCamera::~StripPhotoCamera(void) {

	}//Destructor


	void StripPhotoCamera::init(void) {
		clear();

		m_StripPhoto.init(1.5f, 1000);

		m_Camera.open(0);
		if (!m_Camera.isOpened()) {
			throw CForgeExcept("Failed to open camera device 0");
		}
		else {
			m_Camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1280);
			m_Camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 720);
		}

		if (!m_Camera.get(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB)) {
			throw CForgeExcept("Camera does not support convert RGB");
		}

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

		m_LastFrame = CVLine;

		// count FPS
		/*m_FPSCounter++;
		if (Utilities::timestampMilliseconds() - m_LastFPSPrint > 1000) {
			printf("FPS: %d\n", m_FPSCounter);
			m_LastFPSPrint = Utilities::timestampMilliseconds();
			m_FPSCounter = 0;
		}*/

		
	}//update

	cv::Mat StripPhotoCamera::lastFrame(void) {
		return m_LastFrame;
	}

}//name-space