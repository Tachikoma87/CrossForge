#include <stdio.h>

// OpenCV includes
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include "CForge/Core/SGPIO.h"
#include "Prototypes/I2C.h"
#include "Prototypes/MPU6050.h"
#include "Prototypes/RotaryEncoder.h"
#include "Prototypes/PhotoFinish.h"
#include "Prototypes/StripPhotoCamera.h"

using namespace CForge;

int blinkTest(void) {
	const uint8_t LEDPin = 252;
	GPIO::pinMode(LEDPin, GPIO::OUTPUT, GPIO::HIGH);

	for (int8_t i = 0; i < 5; ++i) {
		GPIO::digitalWrite(LEDPin, GPIO::HIGH);
#ifdef _WIN32
		Sleep(1);
#else
		sleep(1);
#endif
		GPIO::digitalWrite(LEDPin, GPIO::LOW);
#ifdef _WIN32
		Sleep(1);
#else
		sleep(1);
#endif

		printf("Blink %d (%d)\n", i, GPIO::digitalRead(LEDPin));
	}

	return 0;
}//blinkTest

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

void rotaryEncoderTest(void) {
	printf("Starting rotary encoder test\n");

	auto Start = std::chrono::system_clock::now();
	auto End = Start + std::chrono::seconds(5);

	RotaryEncoder Enc;
	Enc.begin(161u, 160u, 188u);
	printf("Encoder started\n");
	GPIO::pinMode(166, GPIO::OUTPUT);
	bool On = false;

	int32_t Pos = Enc.position();

	while (std::chrono::system_clock::now() < End) {
		Enc.update();

		if (Pos != Enc.position()) {
			printf("Position: %d\n", Enc.position());
			Pos = Enc.position();
		}

		if (Enc.buttonState()) {
			printf("Button pressed!\n");
			Enc.resetPosition();

			try {
				On = !On;

				if (On) GPIO::digitalWrite(166, GPIO::HIGH);
				else GPIO::digitalWrite(166, GPIO::LOW);
				std::this_thread::sleep_for(std::chrono::milliseconds(250));

			}
			catch (...) {
				printf("Error occured setting pin mode\n");
			}

		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}

	printf("End rotary encoder test\n");

}//rotaryEncoderTest

cv::Mat augmentLine(cv::Mat M, int32_t x, int32_t y) {
	cv::Mat AM = M;
	for (int32_t i = 0; i < M.rows; ++i) {

		AM.at<cv::Vec3b>(i, x)[0] = 0;
		AM.at<cv::Vec3b>(i, x)[1] = 0;
		AM.at<cv::Vec3b>(i, x)[2] = 255;

	}
	return AM;
}//augmentLine

CrossForge::PhotoFinish::FinishLine_t getLine(cv::Mat M, int32_t x) {
	CrossForge::PhotoFinish::FinishLine_t Rval;
	Rval = CrossForge::PhotoFinish::FinishLine_t(720, 1);

	for (int32_t i = 0; i < M.rows; ++i) {
		cv::Vec3b Val = M.at<cv::Vec3b>(i, x);
		Rval.coeffRef(i, 0).x() = Val[0];
		Rval.coeffRef(i, 0).y() = Val[1];
		Rval.coeffRef(i, 0).z() = Val[2];
	}
	return Rval;
}//getLine

void cameraTest(void) {
	// open the first webcam plugged in the computer
	cv::VideoCapture camera(0);
	if (!camera.isOpened()) {
		throw CForgeExcept("Unable to open camera!");
	}
	else {
		printf("Camera successfully opened!\n");
		camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1280);
		camera.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 720);
		//if (!camera.set(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB, )) printf("Camera RGB Mode not supported\n");

		bool Rval = camera.get(cv::VideoCaptureProperties::CAP_PROP_CONVERT_RGB);
		printf("RGB: %d\n", Rval);
	}

	// create PhotoFinis
	CrossForge::PhotoFinish FF;
	FF.init(720);

	// create a window to display the images from the webcam
	cv::namedWindow("Webcam");

	// this will contain the image from the webcam
	cv::Mat frame;

	// capture the next frame from the webcam
	camera.read(frame);

	int32_t LineX = 1280 / 2;
	int32_t LineStep = 1;

	while (1) {
		// show the image on the windows
		bool FrameAvailable = camera.read(frame);

		if (FrameAvailable && frame.rows > 0) {
			float Error = 0.0f;
			CrossForge::PhotoFinish::FinishLine_t FL = getLine(frame, LineX);
			FF.add(FL, 1.0f, &Error);
			if (Error > 1.5f) printf("Finish line crossed! (%.5f)\n", Error);
			frame = augmentLine(frame, LineX, 0);
			cv::imshow("Webcam", frame);
		}
		// wait (10ms) for a key to be pressed
		if (cv::waitKey(0) >= 0) break;
	}//while[show image]

	cv::destroyWindow("Webcam");


}//cameraTest

void stripPhotoCameraTest(void) {
	StripPhotoCamera Cam;
	Cam.init();
	cv::Mat Frame;
	cv::namedWindow("WinWebcam");

	while (1) {
		Cam.update();

		Frame = Cam.lastFrame();
		cv::imshow("WinWebcam", Frame);

		if (cv::waitKey(1) >= 0) break;

	}

	cv::destroyWindow("WinWebcam");
}//StripPhotoCameraTest