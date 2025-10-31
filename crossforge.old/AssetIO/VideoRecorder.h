/*****************************************************************************\
*                                                                           *
* File(s): VideoRecorder.h and VideoRecorder.cpp                            *
*                                                                           *
* Content: A video recorder class based on ffmpeg that creates a video      *
*          stream from a series of images.                                  *
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
#ifndef __CFORGE_VIDEORECORDER_H__
#define __CFORGE_VIDEORECORDER_H__

#include "T2DImage.hpp"

namespace CForge {
	/**
	* \brief A video recorder class based on ffmpeg that creates a video stream from a series of images.
	* \ingroup AssetIO
	* 
	* \todo Make this class optional to render ffmpeg optional.
	* \todo Move encoding into separate thread to increase performance.
	* \todo Create example that shows usage of this class.
	* \todo Review timestamp parameter and make sure it is handled correctly. Add to addFrame.
	* \todo Change pass by pointer to pass by reference.
	*/
	class CFORGE_API VideoRecorder : public CForgeObject {
	public:
		/**
		* \brief Constructor
		*/
		VideoRecorder(void);

		/**
		* \brief Destructor
		*/
		~VideoRecorder(void);

		/**
		* \brief Initialization method.
		*/
		void init();

		/**
		* \brief Clear method.
		*/
		void clear(void);

		/**
		* \brief Starts the recording.
		* 
		* \param[in] Filename Path to the file where the video will be stored.
		* \param[in] Width Frame width.
		* \param[in] Height Frame height.
		* \param[in] FPS Desired number of frames per second.
		*/
		void startRecording(const std::string Filename, uint32_t Width, uint32_t Height, const float FPS = 30.0f);

		/**
		* \brief Stops the recording and finishes writing the file.
		*/
		void stopRecording();

		/**
		* \brief Add another frame to the stream.
		* 
		* Currently it is expected that you add the exact number of frames per second that you specified then calling startRecording.
		* \param[in] pImg The image to add. Will be scaled so it fits the video width and height.
		*/
		void addFrame(const T2DImage<uint8_t>* pImg);

		/**
		* \brief Getter for the frames per second (fps) value.
		* 
		* \return Frames per second of the video stream.
		*/
		float fps(void)const;

		/**
		* \brief Getter for the width.
		* 
		* \return Width of the video stream.
		*/
		uint32_t width(void)const;

		/**
		* \brief Getter for the height.
		* 
		* \return Height of the video stream.
		*/
		uint32_t height(void)const;

		/**
		* \brief Returns whether recording is active or not.
		* 
		* \return True if recording is active, false otherwise.
		*/
		bool isRecording(void)const;

	protected:
		struct VideoData* m_pData;	///< Internal data of the stream.
	};//VideoRecorder

}//name-space

#endif 
