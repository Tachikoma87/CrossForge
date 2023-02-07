/*****************************************************************************\
*                                                                           *
* File(s): PhotoFinish.h and PhotoFinish.cpp                *
*                                                                           *
* Content:                 *
*                                                                           *
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
#pragma once

#include <inttypes.h>
#include <Eigen/Eigen>

namespace CrossForge {
	class PhotoFinish {
	public:
		typedef Eigen::Matrix<Eigen::Matrix<uint8_t, 3, 1>, -1, -1> FinishLine_t;


		PhotoFinish(void);
		~PhotoFinish(void);

		void init(int16_t LineHeight, int16_t LineWidth = 1);
		void clear(void);

		void add(FinishLine_t Line, float Timing, float *pErrorValue);
		void noiseThreshold(float Val);

	protected:
		std::vector<FinishLine_t> m_Lines;
		std::vector<float> m_Timings; ///< Timing values of the finish line in seconds

		float computeError(int32_t PreviousLineID, int32_t CurrentLineID);

		int16_t m_LineHeight;
		int16_t m_LineWidth;
		float m_NoiseThreshold;

	};//OpticalLightBarrier

}//name space