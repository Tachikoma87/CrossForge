#ifdef __linux__
#include <gpiod.h>
#endif
#include "SGPIO.h"
#include "../Core/CrossForgeException.h"

namespace CForge {

	SGPIO* SGPIO::m_pInstance = nullptr;
	uint32_t SGPIO::m_InstanceCount = 0;

#ifdef __linux__
	void SGPIO::pinMode(int8_t Pin, int8_t Mode) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("GPIO not properly initialized!");
		gpiod_line *pHandle = m_pInstance->openLine(Pin);
		int32_t Rval = 0;
		if (OUTPUT == Mode) Rval = gpiod_line_request_output(pHandle, "Consumer", LOW);
		else Rval = gpiod_line_request_input(pHandle, "Consumer");
		if (0 != Rval) throw CForgeExcept("Failed to set mode for pin " + std::to_string(Pin));
	}//pinMode

	void SGPIO::digitalWrite(int8_t Pin, int8_t State) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("GPIO not properly initialized!");
		gpiod_line* pHandle = m_pInstance->openLine(Pin);
		if (0 != gpiod_line_set_value(pHandle, State)) throw CForgeExcept("Failed to set value for pin " + std::to_string(Pin));
	}//digitalWrite

	int8_t SGPIO::digitalRead(int8_t Pin) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("GPIO not properly initialized!");
		gpiod_line* pHandle = m_pInstance->openLine(Pin);
		int8_t Rval = gpiod_line_get_value(pHandle);
		if (Rval == -1) throw CForgeExcept("Failed to read value from pin " + std::to_string(Pin));
		return Rval;
	}//digitalRead

	void SGPIO::clear(void) {
		// release all lines
		for (auto& i : m_Lines) {
			if (nullptr != i) gpiod_line_release(i);
			i = nullptr;
		}//for[all lines]
		m_Lines.clear();
		closeChip();
	}//clear

	void SGPIO::openChip(void) {
		closeChip();
		m_pChip = gpiod_chip_open_by_name(m_Chipname.c_str());
		if (nullptr == m_pChip) throw CForgeExcept("Failed to open GPIO chip " + m_Chipname);

	}//openChip

	void SGPIO::closeChip(void) {
		if (nullptr != m_pChip) gpiod_chip_close(m_pChip);
		m_pChip = nullptr;
	}//closeChip

	gpiod_line* SGPIO::openLine(int8_t PinID) {
		if (PinID < 0) throw IndexOutOfBoundsExcept("PinID must be positive!");
		while (m_Lines.size() <= PinID) m_Lines.push_back(nullptr);
		if (nullptr == m_Lines[PinID]) {
			m_Lines[PinID] = gpiod_chip_get_line(m_pChip, PinID);
			if (nullptr == m_Lines[PinID]) throw CForgeExcept("Unable to open pin " + std::to_string(PinID));
		}
		return m_Lines[PinID];

	}//openLine
#else

	struct gpiod_line {
		int32_t Stub;
	};

	void SGPIO::pinMode(int8_t Pin, int8_t Mode) {
		throw CForgeExcept("GPIO not available on windows!");
	}//pinMode

	void SGPIO::digitalWrite(int8_t Pin, int8_t State) {
		throw CForgeExcept("GPIO not available on windows!");
	}//digitalWrite

	int8_t SGPIO::digitalRead(int8_t Pin) {
		throw CForgeExcept("GPIO not available on windows!");
	}//digitalRead

	void SGPIO::openChip(void) {
		throw CForgeExcept("GPIO not available on windows!");
	}//openChip

	void SGPIO::closeChip(void) {
		throw CForgeExcept("GPIO not available on windows!");
	}//closeChip

	void SGPIO::clear(void) {
		throw CForgeExcept("GPIO not available on windows!");
	}//clear

	gpiod_line* SGPIO::openLine(int8_t PinID) {
		return nullptr;
	}//openLine

#endif

	SGPIO* SGPIO::instance(void) {
		if (nullptr == m_pInstance) m_pInstance = new SGPIO();
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	void SGPIO::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (m_InstanceCount == 0) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	void SGPIO::init(const std::string Chipname) {
		m_Chipname = Chipname;
		openChip();
	}//initialize


	SGPIO::SGPIO(void) {
		m_Chipname = "gpiochip0";
		m_pChip = nullptr;
		init();
	}//Constructor

	SGPIO::~SGPIO(void) {
		clear();
	}//Destructor
	
	

}//name-space