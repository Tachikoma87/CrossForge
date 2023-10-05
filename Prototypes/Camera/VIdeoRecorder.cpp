#ifdef USE_OPENCV

#include <opencv2/opencv.hpp>
#include "VideoRecorder.h"

namespace CForge {

	void convertCFImageToCVImageRGB(cv::Mat* pCVImg, const T2DImage<uint8_t>* pImg) {
		if (nullptr == pCVImg) throw NullpointerExcept("pCVImg");
		if (nullptr == pImg) throw NullpointerExcept("pImg");

		// create appropriate image
		(*pCVImg) = cv::Mat(pImg->height(), pImg->width(), CV_8UC3, cv::Scalar(0, 0, 0));

		// copy data, convert form RGB to BGR
		for (uint32_t y = 0; y < pImg->height(); ++y) {
			for (uint32_t x = 0; x < pImg->width(); x++) {
				const uint8_t* pPixel = pImg->pixel(x, pImg->height() - y - 1);

				pCVImg->at<cv::Vec3b>(y, x)[0] = pPixel[2];
				pCVImg->at<cv::Vec3b>(y, x)[1] = pPixel[1];
				pCVImg->at<cv::Vec3b>(y, x)[2] = pPixel[0];

			}//for[columns]
		}//for[rows]

	}//convertCFImageToCVImageRBB


	VideoRecorder::VideoRecorder(void): CForgeObject("VideoRecorder") {
		m_FrameWidth = 0;
		m_FrameHeight = 0;
		m_FPS = 0.0f;
		m_pVideoWriter = nullptr;
	}//Constructor

	VideoRecorder::~VideoRecorder(void) {
		clear();
	}//Destructor

	void VideoRecorder::init(const std::string Filename, const float FPS) {
		clear();
		m_Filename = Filename;
		m_FPS = FPS;
	}//initialize

	void VideoRecorder::clear(void) {
		if (nullptr != m_pVideoWriter) {
			static_cast<cv::VideoWriter*>(m_pVideoWriter)->release();
			delete m_pVideoWriter;	
		}
		m_pVideoWriter = nullptr;
		m_FrameWidth = 0;
		m_FrameHeight = 0;
		m_FPS = 0.0f;

		
	}//clear

	void VideoRecorder::release(void) {
		delete this;
	}//release

	void VideoRecorder::addFrame(const T2DImage<uint8_t>* pImg, uint64_t Timestamp) {

		// first frame determines settings
		if (m_FrameWidth == 0) {
			m_FrameWidth = pImg->width();
			m_FrameHeight = pImg->height();

			if (nullptr != m_pVideoWriter) delete m_pVideoWriter;
			//m_pVideoWriter = new cv::VideoWriter(m_Filename.c_str(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(m_FrameWidth, m_FrameHeight));

			m_pVideoWriter = new cv::VideoWriter(m_Filename.c_str(), cv::VideoWriter::fourcc('h', '2', '6', '4'), m_FPS, cv::Size(m_FrameWidth, m_FrameHeight));
		}

		if (m_FrameWidth != pImg->width() || m_FrameHeight != pImg->height()) throw CForgeExcept("Image dimensions don't match expected dimensions for video.");

		Frame* pFrame = new Frame();
		pFrame->Timestamp = Timestamp;
		pFrame->Img.init(pImg->width(), pImg->height(), pImg->colorSpace(), pImg->data());

		cv::Mat Img;
		convertCFImageToCVImageRGB(&Img, pImg);
		static_cast<cv::VideoWriter*>(m_pVideoWriter)->write(Img);

		delete pFrame;
	}//addFrame

	void VideoRecorder::finish(void) {
		clear();
	}//finish

	float VideoRecorder::fps(void)const {
		return m_FPS;
	}//fps

	uint32_t VideoRecorder::width(void)const {
		return m_FrameWidth;
	}//width

	uint32_t VideoRecorder::height(void)const {
		return m_FrameHeight;
	}//height

	bool VideoRecorder::isRecording(void)const {
		return (m_FrameWidth != 0);
	}//isRecording

}//name space

#endif // USE_OPENCV