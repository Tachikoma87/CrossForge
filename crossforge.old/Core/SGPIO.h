/*****************************************************************************\
*                                                                           *
* File(s): SGPIO.h and SGPIO.cpp                                            *
*                                                                           *
* Content: General purpose input-output (GPIO) class for Raspberry Pi and   *
*          similar boards.                                                  *
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

#elif defined(__unix__) && defined(__arm__)
#include <gpiod.h>
#endif

#include "CForgeObject.h"

namespace CForge {

	/**
	* \brief General purpose input-output (GPIO) class for the Raspberry Pi (and similar boards).
	* 
	* \ingroup Core
	* \todo Add interrupt system (polling thread with sleep of 2 milliseconds)
	* \todo Write tutorial on how to use GPIO with Raspberry Pi.
	*/
	class CFORGE_API  SGPIO: public CForgeObject {
	public:
		static const int8_t INPUT = 0;			///< Define for input.
		static const int8_t OUTPUT = 1;			///< Define for output.
		static const int8_t INPUT_PULLUP = 2;	///< Define for input pullup.
		static const int8_t INPUT_PULLDOWN = 3;	///< Define for input pulldown.

		static const int8_t LOW = 0;	///< Define for state low.
		static const int8_t HIGH = 1;	///< Define for state high.

		static const int8_t RISING = 0;	///< Define for rising flank.
		static const int8_t FALLING = 1;///< Define for falling flank.
		static const int8_t CHANGE = 2;	///< Define for changing flank.

		/**
		* \brief Configure pint mode.
		* \param[in] Pin Pin identifier.
		* \param[in] Mode Mode to set (INPUT or OUTPUT).
		* \param[in] Default output value (LOW/HIGH). Only applicable if Mode = OUTPUT.
		*/
		static void pinMode(uint8_t Pin, int8_t Mode, int8_t DefaultOutput = LOW);

		/**
		* \brief Set value of a pin.
		* \param[in] Pin Pin identifier.
		* \param[in] State New value (LOW/HIGH).
		*/
		static void digitalWrite(uint8_t Pin, int8_t State);

		/**
		* \brief Ready value from a pin.
		* \param[in] Pin Pin identifier.
		* \return Read value (LOW/HIGH).
		*/
		static int8_t digitalRead(uint8_t Pin);	

		/**
		* \brief Initialization method. Has to be called once with the appropriate chip identifier.
		* \param[in] Chipname Chip identifier. Default value for the Raspberry Pi system.
		*/
		void init(const std::string Chipname = "gpiochip0");

		/**
		* \brief Singleton instance method.
		* \return Reference of the class's instance.
		*/
		static SGPIO* instance();

		/**
		* \brief Getter for the number of active instances.
		* \return Number of active instances.
		*/
		static int32_t instanceCount();

		/**
		* \brief Singleton's release method.
		*/
		void release();

	protected:

		/**
		* \brief Constructor
		*/
		SGPIO(void);

		/**
		* \brief Destructor
		*/
		~SGPIO(void);
	
		/**
		* \brief Clear method.
		*/
		void clear(void);

	private:
		static SGPIO* m_pInstance;			///< Unique instance pointer.
		static uint32_t m_InstanceCount;	///< Number of active instances.

		/**
		* \brief Establish connection with the GPIO chip.
		*/
		void openChip(void);

		/**
		* \brief End connection with the GPIO chip.
		*/
		void closeChip(void);

		/**
		* \brief Terminate connection with an I/O line.
		*/
		void closeLine(uint8_t PinID);

		/**
		* \brief Check whether a line is in use or not.
		* \param[in] PinID Line to check.
		* \return Whether specified line is in use (true) or not (false).
		*/
		static bool lineInUse(uint8_t PinID);

		std::string m_Chipname;	///< System identifier of the chip.

#if defined(USE_SYSFS_GPIO)
		struct GPIOLine {
			uint8_t ID;						///< Identifier
			std::string ValueStream;		///< URI to ready/write values.
			std::string DirectionStream;	///< URI to select direction (input/output).
		};
		std::vector<GPIOLine*> m_Lines;		///< List of active lines.

		/**
		* \brief Establish connection with an I/O line.
		* \param[in] PinID Line identifier.
		* \return Access to the I/O line.
		*/
		GPIOLine* openLine(uint8_t PinID);
#else
		class gpiod_chip* m_pChip;
		std::vector<struct gpiod_line*> m_Lines;

		/**
		* \brief Establish connection with an I/O line.
		* \param[in] PinID Line identifier.
		* \return Access to the I/O line.
		*/
		gpiod_line* openLine(int8_t PinID);
#endif
	
	};//CGPIO

	typedef SGPIO GPIO;
}//name-space