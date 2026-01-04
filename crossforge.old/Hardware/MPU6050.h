/*****************************************************************************\
*                                                                           *
* File(s): CMPU6050.h and CMPU6050.cpp                                              *
*                                                                           *
* Content:    *
*          .                                         *
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

#include <inttypes.h>
#include <crossforge/Core/SGPIO.h>
#include "I2C.h"

namespace CForge {
	class MPU6050 {
	public:
		struct RawSensorData {
			int16_t AccelX, AccelY, AccelZ;
			int16_t GyroX, GyroY, GyroZ;
			int16_t Temperature;
		};

		struct SensorData {
			float AccelX, AccelY, AccelZ;
			float GyroX, GyroY, GyroZ;
			float Temperature;
		};

		enum struct GyroConfig : int8_t {
			SCALE_250 = 0,
			SCALE_500,
			SCALE_1000,
			SCALE_2000,
		};

		enum struct AccelConfig : int8_t {
			SCALE_2G = 0,
			SCALE_4G,
			SCALE_8G,
			SCALE_16G,
		};

		MPU6050(void);
		~MPU6050(void);

		void begin(I2C* pI2C, int16_t Adr = 0x68, AccelConfig = AccelConfig::SCALE_2G, GyroConfig GC = GyroConfig::SCALE_250);
		void end(void);

		void sleep(void);
		void wake(uint32_t WaitAfterWake = 50);

		void configAccel(AccelConfig Value, bool Recalibrate = true);
		void configGyro(GyroConfig Value, bool Recalibrate = true);
		void configThermistor(bool Enable);

		RawSensorData readRaw(void);
		SensorData read(void);

		void reset(uint32_t WaitAfterReset = 100);

		void calibrate(void);
		void calibrate(const RawSensorData CalibrationData);

	protected:
		int16_t accelSensivity(AccelConfig Value);
		float gyroSensivity(GyroConfig Value);

		int16_t m_SensorAdr;
		I2C* m_pWire;
		AccelConfig m_AccelConfig;
		GyroConfig m_GyroConfig;
		RawSensorData m_Calibration;
	};//CMPU6050


}//name-space