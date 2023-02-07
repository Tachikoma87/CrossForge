/*****************************************************************************\
*                                                                           *
* File(s): CI2C.h and CI2C.cpp                                              *
*                                                                           *
* Content: Class to interact with an MF52 NTC Thermistor by using a basic   *
*          voltage divider circuit.                                         *
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

#if defined(__EMSCRIPTEN__)

#elif defined(__unix__)
#include <gpiod.h>
#endif

#include "CForgeObject.h"

namespace CForge {

	/**
	* \brief General purpose input-output (GPIO) class for the Raspberry Pi (and similar boards).
	* 
	* \todo Add interrupt system (polling thread with sleep of 2 milliseconds)
	* \todo Do full documentation
	*/
	class CFORGE_API  SGPIO: public CForgeObject {
	public:
		static const int8_t INPUT = 0;
		static const int8_t OUTPUT = 1;
		static const int8_t INPUT_PULLUP = 2;
		static const int8_t INPUT_PULLDOWN = 3;

		static const int8_t LOW = 0;
		static const int8_t HIGH = 1;

		static const int8_t RISING = 0;
		static const int8_t FALLING = 1;
		static const int8_t CHANGE = 2;

		static void pinMode(uint8_t Pin, int8_t Mode, int8_t DefaultOutput = LOW);
		static void digitalWrite(uint8_t Pin, int8_t State);
		static int8_t digitalRead(uint8_t Pin);	

		void init(const std::string Chipname = "gpiochip0");

		static SGPIO* instance(void);
		void release(void);

	protected:

		SGPIO(void);
		~SGPIO(void);
	
		void clear(void);

	private:
		static SGPIO* m_pInstance;
		static uint32_t m_InstanceCount;

		void openChip(void);
		void closeChip(void);

		
		void closeLine(uint8_t PinID);

		static bool lineInUse(uint8_t PinID);

		std::string m_Chipname;

#if defined(USE_SYSFS_GPIO)
		struct GPIOLine {
			uint8_t ID;
			std::string ValueStream;
			std::string DirectionStream;
		};
		std::vector<GPIOLine*> m_Lines;

		GPIOLine* openLine(uint8_t PinID);
#else
		class gpiod_chip* m_pChip;
		std::vector<struct gpiod_line*> m_Lines;

		gpiod_line* openLine(int8_t PinID);
#endif
	
	};//CGPIO

	typedef SGPIO GPIO;
}//name-space