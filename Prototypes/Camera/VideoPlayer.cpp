#include "VideoPlayer.h"

#include <opencv2/opencv.hpp>

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


	VideoPlayer::VideoPlayer(void): CForgeObject("VideoPlayer") {
		m_pVideoCapture = nullptr;
	}//Constructor

	VideoPlayer::~VideoPlayer(void) {
		clear();
	}//Destructor

	void VideoPlayer::init(Eigen::Vector2f Position, Eigen::Vector2f Size) {
		m_Position = Position;
		m_Size = Size;

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

	}//clear

	void VideoPlayer::release(void) {
		delete this;
	}//release

	void VideoPlayer::update(void) {
		if (nullptr == m_pVideoCapture) return; // nothing to do
		cv::VideoCapture* pVC = static_cast<cv::VideoCapture*>(m_pVideoCapture);

		uint64_t Timestep = uint64_t(1000.0 / pVC->get(cv::CAP_PROP_FPS));
		while (nullptr != m_pVideoCapture && CForgeUtility::timestamp() > m_VideoStart + m_FrameTimestamp + Timestep) readNextFrame();

	}//update

	void VideoPlayer::play(const std::string VideoFile) {
		// open video file
		cv::VideoCapture* pVideoCapture = new cv::VideoCapture(VideoFile.c_str());
		if (!pVideoCapture->isOpened()) throw CForgeExcept("Video file " + VideoFile + " could not be opened!");

		m_pVideoCapture = pVideoCapture;

		readNextFrame();

		m_VideoStart = CForgeUtility::timestamp();

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
	}//canvasSize

	void VideoPlayer::computeDisplayParams(void) {
		// if values less than 1.0 relative values in percentage assumed, absolute values otherwise
		// compute scale values
		if (m_Size.x() <= 1.0f) {
			m_DisplayScale.x() = m_Size.x();
		}
		else {

		}

		if (m_Size.y() <= 1.0f) {
			m_DisplayScale.y() = m_Size.y();
		}
		else {

		}

		// compute translation values
		if (m_Position.x() <= 1.0f) {
			m_DisplayPosition.x() = 2.0f*m_Position.x() - (1.0f - m_DisplayScale.x());
		}
		else {

		}

		if (m_Position.y() <= 1.0f) {
			m_DisplayPosition.y() = ( -2.0f * m_Position.y() + (1.0f-m_DisplayScale.y()));
		}
		else {

		}

	}//computeDisplayParams
}//name space