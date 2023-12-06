#include "VideoPlayer.h"

#ifdef INCLUDE_OPENCV
#include <opencv2/opencv.hpp>
#endif

#include <crossforge/Utility/CForgeUtility.h>


#include <crossforge/AssetIO/SAssetIO.h>
#include <crossforge/Graphics/Shader/SShaderManager.h>
#include <crossforge/Graphics/RenderDevice.h>
#include <glad/glad.h>

using namespace Eigen;

/**
Plan:
	1. [x] display single texture on screen
	2. [x] display several textures in succession (buffer size)
	3. [x] load video and set frames as textures (buffering)
	4. [ ] implement multi-threaded buffer system 
	5. test and see if that fulfills all requirements
*/ 


namespace CForge {

#ifdef INCLUDE_OPENCV
	void convertCVImageToCFImageRGB(const cv::Mat* pCVImg, T2DImage<uint8_t>* pImg) {
		// convert to Image2D
		uint8_t* pImgData = new uint8_t[pCVImg->rows * pCVImg->cols * 3];

		for (uint32_t y = 0; y < pCVImg->rows; ++y) {
			for (uint32_t x = 0; x < pCVImg->cols; ++x) {
				const cv::Vec3b Pixel = pCVImg->at<cv::Vec3b>(y, x);
				const uint32_t Index = ((pCVImg->rows - y - 1) * pCVImg->cols + x) * 3;
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

#endif


	VideoPlayer::VideoPlayer(void): CForgeObject("VideoPlayer") {
		m_pVideoCapture = nullptr;
		m_FinishedPlaying = true;
	}//Constructor

	VideoPlayer::~VideoPlayer(void) {
		clear();
	}//Destructor

	void VideoPlayer::init(Eigen::Vector2f Position, Eigen::Vector2f Size, Eigen::Vector2i CanvasSize) {
		m_Position = Position;
		m_Size = Size;
		m_CanvasSize = CanvasSize;

		// create shader
		SShaderManager* pSMan = SShaderManager::instance();

		std::vector<ShaderCode*> VSSources;
		std::vector<ShaderCode*> FSSources;
		VSSources.push_back(pSMan->createShaderCode("Shader/ScreenQuad.vert", "330 core", 0, ""));
		FSSources.push_back(pSMan->createShaderCode("Shader/ScreenQuad.frag", "330 core", 0, ""));
		std::string ErrorLog;

		m_pDisplayQuadShader = pSMan->buildShader(&VSSources, &FSSources, &ErrorLog);

		if (!ErrorLog.empty()) throw CForgeExcept("Error building video player shader: \n" + ErrorLog);

		m_DisplayQuad.init(0.0f, 0.0f, 1.0f, 1.0f, nullptr);

		computeDisplayParams();

	}//initialize

	void VideoPlayer::clear(void) {
		if (m_VideoBuffer.size() > 0) {
			for (auto& i : m_VideoBuffer) {
				delete i;
				i = nullptr;
			}
		}
		m_VideoBuffer.clear();
	}//clear

	void VideoPlayer::release(void) {
		delete this;
	}//release

	void VideoPlayer::update(void) {
		/*if (m_FinishedPlaying) return;
		cv::VideoCapture* pVC = static_cast<cv::VideoCapture*>(m_pVideoCapture);*/

		//uint64_t Timestep = uint64_t(1000.0 / pVC->get(cv::CAP_PROP_FPS));
		//while (nullptr != m_pVideoCapture && CForgeUtility::timestamp() > m_VideoStart + m_FrameTimestamp + Timestep) readNextFrame();

		uint64_t Timestep = uint64_t(1000.0 / m_FPS);
		while (!m_FinishedPlaying && CForgeUtility::timestamp() > m_VideoStart + m_FrameTimestamp + Timestep) getNextFrame();

	}//update

#ifdef INCLUDE_OPENCV
	void VideoPlayer::cacheVideo(void) {
		
		// clear Video Buffer
		if (m_VideoBuffer.size() > 0) {
			for (auto& i : m_VideoBuffer) {
				delete i;
				i = nullptr;
			}
		}
		m_VideoBuffer.clear();

		cv::VideoCapture* pVC = static_cast<cv::VideoCapture*>(m_pVideoCapture);
		m_FPS = pVC->get(cv::CAP_PROP_FPS);
		cv::Mat Frame;
		pVC->read(Frame);

		while (!Frame.empty()) {
			FrameItem* pItem = new FrameItem();
			convertCVImageToCFImageRGB(&Frame, &pItem->Img);
			pItem->Timestamp = static_cast<uint64_t>(pVC->get(cv::CAP_PROP_POS_MSEC));
			m_VideoBuffer.push_back(pItem);
			pVC->read(Frame);
		}
	
		pVC->release();
		delete pVC;
		m_pVideoCapture = nullptr;
		m_CurrentFrame = -1;

	}//cacheVideo

	void VideoPlayer::play(const std::string VideoFile) {
		// open video file
		cv::VideoCapture* pVideoCapture = new cv::VideoCapture(VideoFile.c_str());
		if (!pVideoCapture->isOpened()) throw CForgeExcept("Video file " + VideoFile + " could not be opened!");

		m_pVideoCapture = pVideoCapture;

		cacheVideo();
		m_CurrentFrame = -1;
		m_VideoStart = CForgeUtility::timestamp();
		m_FinishedPlaying = false;
	}//play

	void VideoPlayer::readNextFrame(void) {
		if (nullptr == m_pVideoCapture) return; // nothing to do

		cv::VideoCapture* pVC = static_cast<cv::VideoCapture*>(m_pVideoCapture);
		cv::Mat Frame;
		pVC->read(Frame);
		if (Frame.empty()) {
			pVC->release();
			delete pVC;
			m_pVideoCapture = nullptr;
		}
		else {
			T2DImage<uint8_t> Img;
			convertCVImageToCFImageRGB(&Frame, &Img);
			m_Tex.init(&Img, false);
			m_FrameTimestamp = static_cast<uint64_t>(pVC->get(cv::CAP_PROP_POS_MSEC));
		}

	}//readNextFrame

#else
	void VideoPlayer::cacheVideo(void) {

		

	}//cacheVideo

	void VideoPlayer::play(const std::string VideoFile) {
		// open video file
		
	}//play

	void VideoPlayer::readNextFrame(void) {	

	}//readNextFrame
#endif

	void VideoPlayer::getNextFrame(void) {
		if (m_FinishedPlaying) return;

		m_CurrentFrame++;
		if (m_CurrentFrame >= m_VideoBuffer.size()) {
			m_FinishedPlaying = true;
		}
		else {
			m_Tex.init(&m_VideoBuffer[m_CurrentFrame]->Img, false);
			m_FrameTimestamp = m_VideoBuffer[m_CurrentFrame]->Timestamp;
		}
	}//getNextFrame

	void VideoPlayer::play(void) {
		if (m_VideoBuffer.size() > 0) {
			m_FinishedPlaying = false;
			m_CurrentFrame = -1;
			getNextFrame();
			m_VideoStart = CForgeUtility::timestamp();
		}
	}//play

	void VideoPlayer::render(RenderDevice* pRDev) {
		if (m_Tex.width() == 0) return; // no valid image frame

		glDisable(GL_DEPTH_TEST);
		pRDev->activeShader(m_pDisplayQuadShader);
		m_pDisplayQuadShader->bindTexture(GLShader::DEFAULTTEX_ALBEDO, &m_Tex);
		pRDev->requestRendering(&m_DisplayQuad, Quaternionf::Identity(), m_DisplayPosition, m_DisplayScale);

		glEnable(GL_DEPTH_TEST);
	}//render

	void VideoPlayer::canvasSize(const uint32_t Width, const uint32_t Height) {
		m_CanvasSize = Vector2i(Width, Height);
		computeDisplayParams();
	}//canvasSize

	void VideoPlayer::computeDisplayParams(void) {
		// if values less than 1.0 relative values in percentage assumed, absolute values otherwise
		// compute scale values
		if (m_Size.x() <= 1.0f) {
			m_DisplayScale.x() = m_Size.x();
		}
		else {
			m_DisplayScale.x() = m_Size.x() / m_CanvasSize.x();
		}

		if (m_Size.y() <= 1.0f) {
			m_DisplayScale.y() = m_Size.y();
		}
		else {
			m_DisplayScale.y() = m_Size.y() / m_CanvasSize.y();
		}

		// compute translation values
		if (m_Position.x() <= 1.0f) {
			m_DisplayPosition.x() = 2.0f*m_Position.x() - (1.0f - m_DisplayScale.x());
		}
		else {
			float Pos = m_Position.x() / m_CanvasSize.x();
			m_DisplayPosition.x() = 2.0f * Pos - (1.0f - m_DisplayScale.x());
		}

		if (m_Position.y() <= 1.0f) {
			m_DisplayPosition.y() = ( -2.0f * m_Position.y() + (1.0f-m_DisplayScale.y()));
		}
		else {
			float Pos = m_Position.y() / m_CanvasSize.y();
			m_DisplayPosition.y() = (-2.0f * Pos + (1.0f - m_DisplayScale.y()));
		}

	}//computeDisplayParams

	Eigen::Vector2f VideoPlayer::position(void)const {
		Vector2f Rval;
		Rval.x() = (m_Position.x() <= 1.0f) ? m_Position.x() * m_CanvasSize.x() : m_Position.x();
		Rval.y() = (m_Position.y() <= 1.0f) ? m_Position.y() * m_CanvasSize.y() : m_Position.y();
		return Rval;
	}//position

	Eigen::Vector2f VideoPlayer::size(void)const {
		Vector2f Rval;
		Rval.x() = (m_Size.x() < 1.0f) ? m_Size.x() * m_CanvasSize.x() : m_Size.x();
		Rval.y() = (m_Size.y() < 1.0f) ? m_Size.y() * m_CanvasSize.y() : m_Size.y();
		return Rval;
	}//size

	bool VideoPlayer::finished(void)const {
		return m_FinishedPlaying;
	}//finished
	
}//name space
