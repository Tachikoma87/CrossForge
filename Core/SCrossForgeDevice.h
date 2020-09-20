/*****************************************************************************\
*                                                                           *
* File(s): SCrossForgeDevice.h and SCrossForgeDevice.cpp                    *
*                                                                           *
* Content: Main device class of the CrossForge library. Keeps an instance   *
*          of the essential singleton class active during its lifetime.     *
*          Implemented as singleton.                                        *
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

#include <string>
#include <inttypes.h>

namespace CForge {
	/**
	* \brief The main device of CrossForge. It keeps one instance of the Logger and GPIO class active during its lifetime.
	*
	* It is recommended to always have one instance of this class active to ensure the library works correctly.
	* \ingroup Core
	*/
	class SCrossForgeDevice {
	public:

		/**
		* \brief Instantiation method.
		* 
		* \return Pointer to the unique instance.
		*/
		static SCrossForgeDevice* instance(void);

		/** 
		* \brief Release an instance. For every instance call a release call must be made.
		*/
		void release(void);


	protected:
		/**
		* \brief Constructor
		*/
		SCrossForgeDevice(void);

		/**
		* \brief Destructor
		*/
		~SCrossForgeDevice(void);

	private:
		static SCrossForgeDevice* m_pInstance; ///< Unique instance pointer.
		static int16_t m_InstanceCount; ///< Number of instantiation calls

		class SLogger* m_pLogger; ///< Logger instance.
		class SGPIO* m_pGPIO; ///< GPIO instance

	};//SCrossForgeDevice
}//name-space