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
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#pragma once

#include <inttypes.h>
#include <string>
#include <vector>

namespace CForge {
	class SGPIO {
	public:
		static const int8_t INPUT = 0;
		static const int8_t OUTPUT = 1;
		static const int8_t LOW = 0;
		static const int8_t HIGH = 1;

		static void pinMode(int8_t Pin, int8_t Mode);
		static void digitalWrite(int8_t Pin, int8_t State);
		static int8_t digitalRead(int8_t Pin);

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

		struct gpiod_line* openLine(int8_t PinID);

		std::string m_Chipname;
		struct gpiod_chip* m_pChip;
		std::vector<struct gpiod_line*> m_Lines;
	
	};//CGPIO

	typedef SGPIO GPIO;
}//name-space