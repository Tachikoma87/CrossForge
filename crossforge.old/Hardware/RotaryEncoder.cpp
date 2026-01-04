#include "RotaryEncoder.h"
#include <crossforge/Core/CrossForgeException.h>
#include <crossforge/Core/SGPIO.h>

namespace CForge {

	RotaryEncoder::RotaryEncoder(void) {
		m_Clk = 0;
		m_Dir = 0;
		m_Switch = 0;
		m_LastState = 0;
		m_RotaryPosition = 0;
		m_Dir = 0;
		m_SwitchState = false;
	}//Constructor

	RotaryEncoder::~RotaryEncoder(void) {
		end();
	}//Destructor

	void RotaryEncoder::begin(uint8_t ClkPin, uint8_t DtPin, uint8_t SwitchPin, bool SwitchPullup) {
		end();

		m_Clk = ClkPin;
		m_Dt = DtPin;
		m_Switch = SwitchPin;

		GPIO::pinMode(m_Clk, GPIO::INPUT);
		GPIO::pinMode(m_Dt, GPIO::INPUT);
		(SwitchPullup) ? GPIO::pinMode(m_Switch, GPIO::INPUT_PULLUP) : GPIO::pinMode(m_Switch, GPIO::INPUT);

		m_RotaryPosition = 0;
		m_LastState = GPIO::digitalRead(m_Clk);

	}//begin

	void RotaryEncoder::end(void) {
		m_Clk = 0;
		m_Dir = 0;
		m_Switch = 0;
		m_LastState = 0;
		m_RotaryPosition = 0;
		m_Dir = 0;
		m_SwitchState = false;
	}//end

	void RotaryEncoder::update(void) {
		m_SwitchState = (GPIO::digitalRead(m_Switch) == GPIO::LOW) ? true : false;

		int8_t State = GPIO::digitalRead(m_Clk);
		if (m_LastState != State) {
			const int8_t Dt = GPIO::digitalRead(m_Dt);
			// determine direction of turn
			if (Dt == State) {
				//rotary encoder turned CCW
				m_Dir = DIR_CW;
				if (m_LastState == GPIO::LOW && Dt == GPIO::HIGH) m_RotaryPosition--;
			}
			else {
				// rotary encoder turned cw
				m_Dir = DIR_CCW;
				if (m_LastState == GPIO::HIGH && Dt == GPIO::HIGH) m_RotaryPosition++;
			}

		}//if[clock signal changed]
		
		m_LastState = State;
	}//update

	bool RotaryEncoder::buttonState(void)const {
		return m_SwitchState;
	}//buttonState

	int8_t RotaryEncoder::direction(void)const {
		return m_Dir;
	}//direction

	int32_t RotaryEncoder::position(void)const {
		return m_RotaryPosition;
	}//position

	int32_t RotaryEncoder::resetPosition(void) {
		const int32_t Rval = m_RotaryPosition;
		m_RotaryPosition = 0;
		return Rval;
	}//resetPosition

}//name-space