/*****************************************************************************\
*                                                                           *
* File(s): RotaryEncoder.h and RotaryEncoder.cpp                            *
*                                                                           *
* Content: This class handles an incremental rotary encoder.                *
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

namespace CForge {
	class RotaryEncoder {
	public:
		static const int8_t DIR_CW = 0;
		static const int8_t DIR_CCW = 1;

		RotaryEncoder(void);
		~RotaryEncoder(void);

		void begin(uint8_t ClkPin, uint8_t DtPin, uint8_t SwitchPin, bool SwitchPullup = true);
		void end(void);
		void update(void);

		bool buttonState(void)const;
		int8_t direction(void)const;
		int32_t position(void)const;
		int32_t resetPosition(void);
	protected:
		uint8_t m_Clk; ///< Clk pin.
		uint8_t m_Dt; ///< Dt pin.
		uint8_t m_Switch; ///< Switch's button.
		int8_t m_LastState; ///< Internal tracking of clk signal.

		int32_t m_RotaryPosition; ///< Current position.
		int8_t m_Dir; ///< Direction the encoder was turned last.
		bool m_SwitchState; ///< State of the switch. True if pressed, false otherwise.
	};//RoatryEncoder

}//name space