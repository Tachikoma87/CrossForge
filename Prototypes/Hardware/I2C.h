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

#include <string>
#include <crossforge/Core/CrossForgeException.h>

namespace CForge {

	/**
	*
	*
	*
	* 
	*/
	class I2C {
	public:
		I2C(void);
		~I2C(void);

		void begin(const std::string Device = "/dev/i2c-1");
		void end(void);

		void beginTransmission(uint8_t SlaveAdr);

		void read(uint8_t SlaveAdr, void* pBuffer, int32_t ByteCount);
		void write(uint8_t SlaveAdr, const void* pBuffer, int32_t ByteCount);
		void writeRead(uint8_t SlaveAdr, const void* pWrite, const int16_t WriteByteCount, void* pRead, int16_t ReadByteCount);

		int8_t registerRead(uint8_t SlaveAdr, uint8_t Register);
		void registerWrite(uint8_t SlaveAdr, uint8_t RegAdr, uint8_t Value);

		
		template <typename T> 
		void write(uint8_t SlaveAdr, T Value) {
			return write(SlaveAdr, &Value, sizeof(T));
		}//writeBus

		template<typename T>
		void read(uint8_t SlaveAdr, T* pValue) {
			if (nullptr == pValue) throw NullpointerExcept("pValue");
			return read(SlaveAdr, pValue, sizeof(T));
		}//readBus

		template<typename T, typename U>
		void writeRead(uint8_t SlaveAdr, const T Write, U *pRead) {
			if (nullptr == pRead) throw NullpointerExcept("pRead!");
			writeRead(SlaveAdr, &Write, sizeof(T), (void*)pRead, sizeof(U));
		}//writeRead

	protected:
		int32_t m_BusDevice;
	};//CI2C

}//name-space