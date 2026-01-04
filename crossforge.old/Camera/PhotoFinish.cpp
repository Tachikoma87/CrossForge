#include "PhotoFinish.h"
#include "../../crossforge/Core/CrossForgeException.h"

namespace CrossForge {
	PhotoFinish::PhotoFinish(void) {

	}//Constructor

	PhotoFinish::~PhotoFinish(void) {

	}//Destructor

	void PhotoFinish::init(int16_t LineHeight, int16_t LineWidth) {
		clear();
		m_LineWidth = LineWidth;
		m_LineHeight = LineHeight;
		m_NoiseThreshold = 15.0f * 15.0f;
	}//initialize

	void PhotoFinish::clear(void) {
		m_LineWidth = 0;
		m_LineHeight = 0;
		m_Lines.clear();
		m_Timings.clear();
	}//clear

	void PhotoFinish::add(FinishLine_t Line, float Timing, float* pErrorValue) {
		m_Lines.push_back(Line);
		m_Timings.push_back(Timing);
		if (nullptr != pErrorValue && m_Lines.size() > 1) (*pErrorValue) = computeError(m_Lines.size() - 2, m_Lines.size() - 1);
	}//add

	float PhotoFinish::computeError(int32_t PreviousLineID, int32_t CurrentLineID) {
		if (CurrentLineID < 0 || CurrentLineID >= m_Lines.size()) throw IndexOutOfBoundsExcept("CurrentLineID");
		if (PreviousLineID < 0 || PreviousLineID >= m_Lines.size()) throw IndexOutOfBoundsExcept("PreviousLineID");
		
		float Rval = 0.0f;

		for (uint16_t k = 0; k < m_LineWidth; ++k) {
			for (uint16_t i = 0; i < m_LineHeight; ++i) {
				// compute squared error
				
				Eigen::Matrix<uint8_t, 3, 1> VPrev = m_Lines[PreviousLineID].coeff(i, k);
				Eigen::Matrix<uint8_t, 3, 1> VCur = m_Lines[CurrentLineID].coeff(i, k);
				Eigen::Matrix<uint8_t, 3, 1> Dist = VPrev - VCur;


				float Error = Dist.dot(Dist);
				if(Error > m_NoiseThreshold) Rval += Error;

			}//for[rows]
		}//for[columns]

		float Scale = (float)(m_LineHeight * m_LineWidth);
		return Rval/Scale;
	}//computeError

	void PhotoFinish::noiseThreshold(float Val) {
		m_NoiseThreshold = Val;
	}//noiseThreshold

}//name space