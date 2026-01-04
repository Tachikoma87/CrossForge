#ifdef __linux__
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#endif

#include "I2C.h"

namespace CForge {

	I2C::I2C(void) {
		m_BusDevice = -1;
	}//Constructor

#ifdef __linux__
	I2C::~I2C(void) {
		end();	
	}//Destructor


	void I2C::begin(const std::string Device) {
		end();
		if (Device.empty()) throw CForgeExcept("Empty device name specified!");
		if ((m_BusDevice = open(Device.c_str(), O_RDWR)) < 0)  throw CForgeExcept("Unable to connect to Bus " + Device);
	}//openConnection

	void I2C::beginTransmission(uint8_t SlaveAdr) {
		if (0 > m_BusDevice) throw NotInitializedExcept("No I2C bus connection available!");
		if (-1 == ioctl(m_BusDevice, I2C_SLAVE, SlaveAdr)) throw CForgeExcept("Unable to connect to I2C Slave " + std::to_string(SlaveAdr));

	}//beginTransmission

	void I2C::end(void) {
		if (m_BusDevice >= 0) close(m_BusDevice);
		m_BusDevice = -1;
	}//closeConnection

	void I2C::read(uint8_t SlaveAdr, void* pBuffer, int32_t ByteCount) {
		if (m_BusDevice < 0) throw NotInitializedExcept("I2C bus or slave not connected!");

		i2c_msg Msg;

		Msg.addr = SlaveAdr;
		Msg.flags = I2C_M_RD;
		Msg.len = ByteCount;
		Msg.buf = (uint8_t*)pBuffer;
		i2c_rdwr_ioctl_data MsgSet;
		MsgSet.nmsgs = 1;
		MsgSet.msgs = &Msg;

		if (ioctl(m_BusDevice, I2C_RDWR, &MsgSet) < 0) throw CForgeExcept("Unable to write to I2C bus!");
	}//read

	void I2C::write(uint8_t SlaveAdr, const void* pBuffer, int32_t ByteCount) {
		if (m_BusDevice < 0) throw NotInitializedExcept("I2C bus or slave not connected!");
		i2c_msg Msg;

		Msg.addr = SlaveAdr;
		Msg.flags = 0;
		Msg.len = ByteCount;
		Msg.buf = (uint8_t*)pBuffer;

		i2c_rdwr_ioctl_data MsgSet;
		MsgSet.nmsgs = 1;
		MsgSet.msgs = &Msg;

		if (ioctl(m_BusDevice, I2C_RDWR, &MsgSet) < 0) throw CForgeExcept("Unable to write to I2C bus!");
	}//write


	void I2C::writeRead(uint8_t SlaveAdr, const void* pWrite, const int16_t WriteByteCount, void* pRead, int16_t ReadByteCount) {
		i2c_msg Msgs[2];

		// write msg
		Msgs[0].addr = SlaveAdr;
		Msgs[0].flags = 0;
		Msgs[0].len = WriteByteCount;
		Msgs[0].buf = (uint8_t*)pWrite;

		Msgs[1].addr = SlaveAdr;
		Msgs[1].flags = I2C_M_RD;
		Msgs[1].len = ReadByteCount;
		Msgs[1].buf = (uint8_t*)pRead;

		i2c_rdwr_ioctl_data MsgSet;
		MsgSet.msgs = Msgs;
		MsgSet.nmsgs = 2;

		if (ioctl(m_BusDevice, I2C_RDWR, &MsgSet) < 0) throw CForgeExcept("Unable to write/read I2C-Bus");

	}//writeReadBus

#else
	I2C::~I2C(void) {

	}//Destructor

	void I2C::begin(const std::string Device) {
		throw CForgeExcept("I2C not available on Windows systems!");
	}//begin

	void I2C::end(void) {
		throw CForgeExcept("I2C not available on Windows systems!");
	}//end

	void I2C::read(uint8_t SlaveAdr, void* pBuffer, int32_t ByteCount) {
		throw CForgeExcept("I2C not available on Windows systems!");
	}//read

	void I2C::write(uint8_t SlaveAdr, const void* pBuffer, int32_t ByteCount) {
		throw CForgeExcept("I2C not available on Windows systems!");
	}//write

	void I2C::writeRead(uint8_t SlaveAdr, const void* pWrite, const int16_t WriteByteCount, void* pRead, int16_t ReadByteCount) {
		throw CForgeExcept("I2C not available on Windows systems!");
	}//writeRead

#endif
	
	int8_t I2C::registerRead(uint8_t SlaveAdr, uint8_t Register) {
		int8_t Rval = 0;
		writeRead(SlaveAdr, &Register, sizeof(uint8_t), &Rval, sizeof(uint8_t));
		return Rval;
	}//registerRead

	void I2C::registerWrite(uint8_t SlaveAdr, uint8_t RegAdr, uint8_t Value) {
		uint8_t Buffer[2] = { RegAdr, Value };
		write(SlaveAdr, Buffer, sizeof(uint8_t) * 2);
	}//registerWrite

}//name-space