#ifdef __linux__
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include "Core/CrossForgeException.h"
#include "Prototypes/SGPIO.h"
#include <memory.h>

#include "Prototypes/I2C.h"
#include "Prototypes/MPU6050.h"
#include "Core/SLogger.h"
#include "Core/SCrossForgeDevice.h"

#include <Eigen/Eigen>

#include <thread>


using namespace CForge;

int blinkTest(void) {
	
	GPIO::pinMode(10, GPIO::OUTPUT);

	for(int8_t i =0; i < 5; ++i){
		GPIO::digitalWrite(10, GPIO::HIGH);
#ifdef _WIN32
		Sleep(1);
#else
		sleep(1);
#endif
		GPIO::digitalWrite(10, GPIO::LOW);
#ifdef _WIN32
		Sleep(1);
#else
		sleep(1);
#endif
		printf("Blink %d\n", i);
	}

	return 0;
}


void MPUTest(void) {

	I2C Bus;
	Bus.begin();

	MPU6050 Gyro;
	Gyro.begin(&Bus, 0x68);

	Gyro.configGyro(MPU6050::GyroConfig::SCALE_1000);
	Gyro.configThermistor(false);
	Gyro.calibrate();

	for (int i = 0; i < 120; ++i) {
		Gyro.wake();
		MPU6050::SensorData Data = Gyro.read();

		/*if (Data.GyroX < -30 || Data.GyroX > 30) printf("Shaken in X direction!\n");
		if (Data.GyroY < -30 || Data.GyroY > 30) printf("Shaen in Y direction!\n");
		if (Data.GyroZ < -30 || Data.GyroZ > 30) printf("Shaken in Z direction!\n");
		usleep(50);*/

		printf("Temp: %.2f | (%.2f %.2f %.2f) | (%.3f %.3f %.3f)\n", Data.Temperature, Data.AccelX, Data.AccelY, Data.AccelZ, Data.GyroX, Data.GyroY, Data.GyroZ);
		//sleep(1);
		Gyro.sleep();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	Bus.end();

	
}

void eigenTest(void) {
	Eigen::Vector3f Vec1, Vec2;
	Vec1.x() = 1.0f;
	Vec1.y() = 2.0f;
	Vec1.z() = 3.0f;
	Vec2 = Vec1;

	printf("Vector 2 contains: %.4f %.4f %.4f\n", Vec2.x(), Vec2.y(), Vec2.z());
	throw CForgeExcept("An intentionally created exception");
}//eigenTest

int main(int argc, char* argv[]) {
	

	SCrossForgeDevice* pDev = SCrossForgeDevice::instance();

	try {
		MPUTest();
	}
	catch (const CrossForgeException & e) {
		SLogger::logException(e);
		printf("Exception occurred. See Log.");
	}
	catch (...) {
		printf("A not handled exception occurred!\n");
	}
	
	pDev->release();
	
	char c;
	scanf("%c", &c);
}//main