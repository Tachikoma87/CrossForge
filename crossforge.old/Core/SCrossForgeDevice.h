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
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#pragma once

#include <mutex>
#include <list>
#include "CForgeObject.h"
#include "CoreDefinitions.h"


namespace CForge {

	/**
	* \brief The main device of CrossForge. It keeps instances of the important singletons alive at all times. It also handles initialization and shutdown of external libraries such as glfw and winsock.
	*
	* It is recommended to always have one instance of this class active to ensure the library works correctly.
	* \ingroup Core
	* 
	* \todo Rewrite destructor using instance count methods of the singletons and make sure everything is properly released.
	*/
	class CFORGE_API SCrossForgeDevice {
	public:

		/**
		* \brief Instantiation method.
		* 
		* \return Pointer to the unique instance.
		*/
		static SCrossForgeDevice* instance(void);

		/**
		* \brief Returns number of active instances.
		* \return Number of active instances.
		*/
		static int32_t instanceCount();

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

		/**
		* \brief Initialization method.
		*/
		void init(void);

		/**
		* \brief Clear method.
		*/
		void clear(void);

	private:
		static SCrossForgeDevice* m_pInstance;	///< Unique instance pointer.
		static int16_t m_InstanceCount;			///< Number of instantiation calls

		SLogger* m_pLogger;					///< Logger instance.
		class SGPIO* m_pGPIO;				///< GPIO instance.
		class SAssetIO* m_pAssIO;			///< Asset importer/exporter instance.
		class STextureManager* m_pTexMan;	///< Texture manager.
		class SShaderManager* m_pSMan;		///< Shader manager.
		class SFontManager* m_pFontMan;		///< Font manager.
		class SCForgeSimulation* m_pSimulation; ///< Simulation object.

	};//SCrossForgeDevice
}//name-space