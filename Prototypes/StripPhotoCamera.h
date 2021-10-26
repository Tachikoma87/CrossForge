/*****************************************************************************\
*                                                                           *
* File(s): StripPhoto.h and StripPhoto.cpp                                  *
*                                                                           *
* Content:    *
*                                                   *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/

#ifndef __CFORGE_STRIPPHOTOCAMERA_H__
#define __CFORGE_STRIPPHOTOCAMERA_H__

#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>
#include "../CForge/Core/CrossForgeException.h"
#include "StripPhoto.h"

namespace CForge {

	class StripPhotoCamera {
	public:
		StripPhotoCamera(void);
		~StripPhotoCamera(void);

		void init(void);
		void clear(void);

		void update(void);
		cv::Mat lastFrame(void);

	protected:
		StripPhoto m_StripPhoto;
		cv::VideoCapture m_Camera;
		cv::Mat m_ImageFrame;

		cv::Mat m_LastFrame;

		uint16_t m_FPSCounter;
		uint64_t m_LastFPSPrint;
	};//StripPhotoCamera

}//name-space

#endif // Header guards