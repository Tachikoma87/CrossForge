#include "MPU6050.h"
#include <crossforge/Core/CrossForgeException.h>
#include <thread>
#include <chrono>

#ifdef __linux__
#include <unistd.h>
#endif

#define MPU6050_DEFAULT_ADR 0x68
#define MPU6050_PWR1 0x6B
#define MPU6050_PWR2 0x6C
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_GYRO_CONFIG 0x1B

using namespace std::chrono_literals;

namespace CForge {

	MPU6050::MPU6050(void) {
		m_pWire = nullptr;
		m_SensorAdr = MPU6050_DEFAULT_ADR;
		m_AccelConfig = AccelConfig::SCALE_2G;
		m_GyroConfig = GyroConfig::SCALE_250;
		m_Calibration = RawSensorData();
	}//Constructor

	MPU6050::~MPU6050(void) {
		end();
	}//Destructor

	void MPU6050::begin(I2C* pI2C, int16_t Adr, AccelConfig AC, GyroConfig GC) {
		if (nullptr == pI2C) throw NullpointerExcept("pI2C was nullptr");
		if (Adr < 0) throw CForgeExcept("Negative I2C address specified. That can't be right");
		m_pWire = pI2C;
		m_SensorAdr = Adr;
		wake(); // wake MPU 
		configAccel(AC, false);
		configGyro(GC, false);
		calibrate();
	}//begin


	void MPU6050::reset(uint32_t WaitAfterReset) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No I2C connection available!");
		m_pWire->registerWrite(m_SensorAdr, MPU6050_PWR1, 0b10000000);
		std::this_thread::sleep_for(std::chrono::milliseconds(WaitAfterReset) );
	}//reset

	void MPU6050::end(void) {
		m_pWire = nullptr; // pointer just borrowed
		m_SensorAdr = MPU6050_DEFAULT_ADR;
		m_AccelConfig = AccelConfig::SCALE_2G;
		m_GyroConfig = GyroConfig::SCALE_250;
		m_Calibration = RawSensorData();
	}//end

	void MPU6050::sleep(void) {
		if (m_pWire == nullptr) throw NotInitializedExcept("No I2C connection available!");
		int8_t Reg = m_pWire->registerRead(m_SensorAdr, MPU6050_PWR1);
		Reg |= 0b01000000;
		m_pWire->registerWrite(m_SensorAdr, 0x6B, Reg);
	}//sleep

	void MPU6050::wake(uint32_t WaitAfterWake) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No i2C connection available!");
		m_pWire->registerWrite(m_SensorAdr, MPU6050_PWR1, 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(WaitAfterWake));
	}//wake

	void MPU6050::configAccel(AccelConfig Value, bool Recalibrate) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No I2C connection available!");

		uint8_t Reg = m_pWire->registerRead(m_SensorAdr, MPU6050_ACCEL_CONFIG);
		Reg &= 0b11100111; // clear configuration bits
		switch (Value) {
		case AccelConfig::SCALE_2G: Reg |= 0b00000000; break;
		case AccelConfig::SCALE_4G: Reg |= 0b00001000; break;
		case AccelConfig::SCALE_8G: Reg |= 0b00010000; break;
		case AccelConfig::SCALE_16G: Reg |= 0b00011000; break;
		default: break;
		}
		m_pWire->registerWrite(m_SensorAdr, MPU6050_ACCEL_CONFIG, Reg);
		m_AccelConfig = Value;
		if (Recalibrate) calibrate();
	}//configAccel

	void MPU6050::configGyro(GyroConfig Value, bool Recalibrate) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No I2C connection available!");

		uint8_t Reg = m_pWire->registerRead(m_SensorAdr, MPU6050_GYRO_CONFIG);
		Reg &= 0b11100111; // clear configuration bits
		switch (Value) {
		case GyroConfig::SCALE_250: Reg |= 0b00000000; break;
		case GyroConfig::SCALE_500: Reg |= 0b00001000; break;
		case GyroConfig::SCALE_1000:Reg |= 0b00010000; break;
		case GyroConfig::SCALE_2000:Reg |= 0b00011000; break;
		default: break;
		}
		m_pWire->registerWrite(m_SensorAdr, 0x1B, Reg);
		m_GyroConfig = Value;
		if (Recalibrate) calibrate();
	}//configGyro


	void MPU6050::configThermistor(bool Enable) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No I2C connection available!");
		uint8_t Reg = m_pWire->registerRead(m_SensorAdr, MPU6050_PWR1);
		if (Enable) Reg &= 0b11110111;
		else Reg |= 0b00001000;
		m_pWire->registerWrite(m_SensorAdr, MPU6050_PWR1, Reg);
	}//configThermistor

	MPU6050::RawSensorData MPU6050::readRaw(void) {
		if (nullptr == m_pWire) throw NotInitializedExcept("No I2C connection available!");
		RawSensorData Rval;

		// read the 14 registers (Accel, Temp, Gyro)
		// 0x3B contains acceleration x data and successive registers contain remaining data
		uint8_t Reg = 0x3B; 
		uint8_t Buffer[14] = { 0 };
		m_pWire->writeRead(m_SensorAdr, &Reg, sizeof(uint8_t), Buffer, sizeof(uint8_t) * 14);

		Rval.AccelX = (Buffer[0] << 8) | Buffer[1];
		Rval.AccelY = (Buffer[2] << 8) | Buffer[3];
		Rval.AccelZ = (Buffer[4] << 8) | Buffer[5];
		Rval.Temperature = (Buffer[6] << 8) | Buffer[7];
		Rval.GyroX = (Buffer[8] << 8) | Buffer[9];
		Rval.GyroY = (Buffer[10] << 8) | Buffer[11];
		Rval.GyroZ = (Buffer[12] << 8) | Buffer[13];

		return Rval;
	}//readRaw

	MPU6050::SensorData MPU6050::read(void) {
		const float AccelSens = accelSensivity(m_AccelConfig);
		const float GyroSens = gyroSensivity(m_GyroConfig);
		RawSensorData Data = readRaw();
		
		// convert to degrees per second. 
		// offset by calibration values
		SensorData Rval;
		Rval.AccelX = (Data.AccelX - m_Calibration.AccelX) / AccelSens;
		Rval.AccelY = (Data.AccelY - m_Calibration.AccelY) / AccelSens;
		Rval.AccelZ = (Data.AccelZ - m_Calibration.AccelZ) / AccelSens;
		Rval.GyroX = (Data.GyroX - m_Calibration.GyroX) / GyroSens;
		Rval.GyroY = (Data.GyroY - m_Calibration.GyroY) / GyroSens;
		Rval.GyroZ = (Data.GyroZ - m_Calibration.GyroZ) / GyroSens;
		Rval.Temperature = Data.Temperature / 340.0f + 36.53f;

		return Rval;
	}//read

	void MPU6050::calibrate(void) {
		m_Calibration = readRaw();
	}//calibrate

	void MPU6050::calibrate(const RawSensorData CalibrationData) {
		m_Calibration = CalibrationData;
	}//calibrate

	int16_t MPU6050::accelSensivity(AccelConfig Value) {
		int16_t Rval = 1;
		switch(Value) {
		case AccelConfig::SCALE_2G: Rval = 16384; break;
		case AccelConfig::SCALE_4G: Rval = 8192; break;
		case AccelConfig::SCALE_8G: Rval = 4096; break;
		case AccelConfig::SCALE_16G: Rval = 2048; break;
		default: Rval = 16384; break;
		}
		return Rval;
	}//accelSensivity

	float MPU6050::gyroSensivity(GyroConfig Value) {
		float Rval = 1.0f;
		switch (Value) {
		case GyroConfig::SCALE_250: Rval = 131.0f; break;
		case GyroConfig::SCALE_500: Rval = 62.5f; break;
		case GyroConfig::SCALE_1000: Rval = 32.6f; break;
		case GyroConfig::SCALE_2000: Rval = 16.4f; break;
		default: Rval = 131.0f; break;
		}
		return Rval;
	}//gyroSensivity

}//name-space