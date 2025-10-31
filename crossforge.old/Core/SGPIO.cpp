
#if defined(__EMSCRIPTEN__)

#elif defined(__linux__) && defined(__arm__)
//#include <gpiod.h>
//#include <pigpio.h>
#include <fcntl.h>
#include <stdlib.h>
#endif

#include "SGPIO.h"
#include "CrossForgeException.h"

namespace CForge {

	SGPIO* SGPIO::m_pInstance = nullptr;
	uint32_t SGPIO::m_InstanceCount = 0;

#if defined(USE_SYSFS_GPIO)
	void SGPIO::pinMode(uint8_t Pin, int8_t Mode, int8_t DefaultOutput) {
		GPIOLine* pLine = m_pInstance->openLine(Pin);
		if (nullptr == pLine) throw CForgeExcept("Unable to open GPIO line " + std::to_string(Pin));
		FILE* pF = fopen(pLine->DirectionStream.c_str(), "w");	
		if (nullptr == pF) throw CForgeExcept("Unable to open direction stream of GPIO line " + std::to_string(Pin));
		fprintf(pF, (Mode == OUTPUT) ? "out" : "in");
		fclose(pF);
		if(Mode == OUTPUT) {
			digitalWrite(Pin, DefaultOutput);
		}
		
	}//pinMode

	void SGPIO::digitalWrite(uint8_t Pin, int8_t State) {
		GPIOLine* pLine = m_pInstance->openLine(Pin);
		if (nullptr == pLine)throw CForgeExcept("Unable to open GPIO line " + std::to_string(Pin));
		FILE* pF = fopen(pLine->ValueStream.c_str(), "w");
		if (nullptr == pF) throw CForgeExcept("Unable to open value stream of GPIO line " + std::to_string(Pin));
		fprintf(pF, (State == LOW) ? "0" : "1");
		fclose(pF);
	}//digitalWrite

	int8_t SGPIO::digitalRead(uint8_t Pin) {
		int8_t Rval = LOW;
		GPIOLine* pLine = m_pInstance->openLine(Pin);
		if (nullptr == pLine) throw CForgeExcept("Unable to open GPIO line " + std::to_string(Pin));
		FILE* pF = fopen(pLine->ValueStream.c_str(), "r");
		fscanf(pF, "%c", &Rval);
		Rval -= '0';
		fclose(pF);
		return Rval;
	}//digitalRead

	void SGPIO::init(const std::string Chipname) {
		m_Chipname = "/sys/class/gpio/";
	}//initialize

	void SGPIO::openChip(void) {
		// not necessary in sysfs
	}//openChip

	void SGPIO::closeChip(void) {
		//not necessary with sysfs
	}//closeChip

	SGPIO::GPIOLine* SGPIO::openLine(uint8_t PinID) {
		GPIOLine* pRval = nullptr;
		
		for (auto i : m_Lines) {
			if (i!= nullptr && i->ID == PinID) {
				pRval = i;
				break;
			}
		}//for[all open lines]

		if (pRval == nullptr) {
			FILE* pF = fopen((m_pInstance->m_Chipname + "export").c_str(), "w");
			if (nullptr == pF) throw CForgeExcept("Unable to open export stream of GPIO.");
			fprintf(pF, "%d", PinID);
			fclose(pF);

			GPIOLine* pLine = new GPIOLine();
			pLine->ID = PinID;
			pLine->DirectionStream = m_Chipname + "gpio" + std::to_string(PinID) + "/direction";
			pLine->ValueStream = m_Chipname + "gpio" + std::to_string(PinID) + "/value";
			m_Lines.push_back(pLine);
			pRval = pLine;
		}
		
		return pRval;
	}//openLine

	void SGPIO::closeLine(uint8_t PinID) {
		for (auto &i : m_Lines) {
			if (i != nullptr && i->ID == PinID) {			
				FILE* pFile = fopen((SGPIO::m_pInstance->m_Chipname + "unexport").c_str(), "w");
				fprintf(pFile, "%d", PinID);
				fclose(pFile);

				delete i;
				i = nullptr;
				break;
			}
		}//for[lines]
	}//closeLine

	bool SGPIO::lineInUse(uint8_t PinID) {
		return true;
	}//lineInUse

	void SGPIO::clear(void) {
		//printf("Have to clear %d lines\n", m_Lines.size());
		for (auto i : m_Lines) {
			if (i != nullptr) closeLine(i->ID);
		}
		m_Lines.clear();
	}//clear

#elif defined(__linux__) && defined(__arm__)

	void SGPIO::pinMode(uint8_t Pin, int8_t Mode, int8_t DefaultOutput) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("GPIO not properly initialized!");
		
		m_pInstance->closeLine(Pin);

		gpiod_line *pHandle = m_pInstance->openLine(Pin);
		int32_t Rval = 0;
		if (OUTPUT == Mode) Rval = gpiod_line_request_output(pHandle, "Consumer", DefaultOutput);
		else if (INPUT_PULLUP == Mode) {
			// activate pull-up resistor using GPIOD_BIT(5)
			Rval = gpiod_line_request_input_flags(pHandle, "Consumer", GPIOD_BIT(5));
		}
		else if (INPUT_PULLDOWN == Mode) {
			Rval = gpiod_line_request_input_flags(pHandle, "Consumer", GPIOD_BIT(4));
		}
		else if (INPUT == Mode) {
			Rval = gpiod_line_request_input(pHandle, "Consumer");
		}
		if (0 != Rval) throw CForgeExcept("Failed to set mode for pin " + std::to_string(Pin));

	}//pinMode

	void SGPIO::digitalWrite(uint8_t Pin, int8_t State) {
		if (nullptr == m_pInstance) throw NotInitializedExcept("GPIO not properly initialized!");
		gpiod_line* pHandle = m_pInstance->openLine(Pin);
		if (0 != gpiod_line_set_value(pHandle, State)) throw CForgeExcept("Failed to set value for pin " + std::to_string(Pin));
	}//digitalWrite

	int8_t SGPIO::digitalRead(uint8_t Pin) {
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

		gpiod_chip_iter* pIter = gpiod_chip_iter_new();
		if (nullptr == pIter) {
			printf("Unable to enumerate valid GPIO devices\n");
			return;
		}

		gpiod_chip* pChip = gpiod_chip_iter_next(pIter);
		if (pChip == nullptr) {
			printf("Unable to find valid GPIO device!\n");
			return;
		}
		

		m_pChip = gpiod_chip_open_by_name(m_Chipname.c_str());
		if (nullptr == m_pChip) throw CForgeExcept("Failed to open GPIO chip " + m_Chipname);

	}//openChip

	void SGPIO::closeChip(void) {
		if (nullptr != m_pChip) gpiod_chip_close(m_pChip);
		m_pChip = nullptr;
	}//closeChip

	gpiod_line* SGPIO::openLine(uint8_t PinID) {
		if (PinID < 0) throw IndexOutOfBoundsExcept("PinID must be positive!");
		while (m_Lines.size() <= PinID) m_Lines.push_back(nullptr);
		if (nullptr == m_Lines[PinID]) {
			m_Lines[PinID] = gpiod_chip_get_line(m_pChip, PinID);
			if (nullptr == m_Lines[PinID]) throw CForgeExcept("Unable to open pin " + std::to_string(PinID));
		}
		return m_Lines[PinID];

	}//openLine

	void SGPIO::closeLine(uint8_t PinID) {
		if (PinID < 0) throw IndexOutOfBoundsExcept("PinID must be positive!");
		if (m_Lines.size() > PinID&& m_Lines[PinID] != nullptr) {
			gpiod_line_release(m_Lines[PinID]);
			m_Lines[PinID] = nullptr;
		}
	}//closeLine

	bool SGPIO::lineInUse(uint8_t PinID) {
		SGPIO* pGPIO = SGPIO::instance();
		bool Rval = (pGPIO->m_Lines.size() > PinID&& pGPIO->m_Lines[PinID] != nullptr) ? true : false;
		pGPIO->release();
		return Rval;
	}//lineInUse

	void SGPIO::init(const std::string Chipname) {
		m_Chipname = Chipname;
		openChip();
	}//initialize

#else

	struct gpiod_line {
		int32_t Stub;
	};

	void SGPIO::pinMode(uint8_t Pin, int8_t Mode, int8_t DefaultOutput) {
		throw CForgeExcept("GPIO not available on windows!");
	}//pinMode

	void SGPIO::digitalWrite(uint8_t Pin, int8_t State) {
		throw CForgeExcept("GPIO not available on windows!");
	}//digitalWrite

	int8_t SGPIO::digitalRead(uint8_t Pin) {
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

	void SGPIO::init(const std::string Chipname) {
		m_Chipname = Chipname;
		openChip();
	}//initialize

#endif

	SGPIO* SGPIO::instance(void) {
		if (nullptr == m_pInstance) m_pInstance = new SGPIO();
		m_InstanceCount++;
		return m_pInstance;
	}//instance

	int32_t SGPIO::instanceCount() {
		return m_InstanceCount;
	}//instanceCount

	void SGPIO::release(void) {
		if (0 == m_InstanceCount) throw CForgeExcept("Not enough instances for a release call!");
		m_InstanceCount--;
		if (m_InstanceCount == 0) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}//release

	
	SGPIO::SGPIO(void): CForgeObject("SGPIO") {
		m_Chipname = "gpiochip0";
#ifndef USE_SYSFS_GPIO
		m_pChip = nullptr;
#endif
		init();
	}//Constructor

	SGPIO::~SGPIO(void) {
		// not on windows
	}//Destructor
	
	

}//name-space