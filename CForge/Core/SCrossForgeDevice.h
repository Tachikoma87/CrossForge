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
	* \brief The main device of CrossForge. It keeps one instance of the Logger and GPIO class active during its lifetime.
	*
	* It is recommended to always have one instance of this class active to ensure the library works correctly.
	* \ingroup Core
	* \todo Do full documentation.
	*/
	class CFORGE_API SCrossForgeDevice {
		friend class CForgeObject;
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

		void init(void);
		void clear(void);

		uint32_t registerObject(CForgeObject* pObj);
		void unregisterObject(CForgeObject* pObj);

		uint32_t retrieveUniqueID(void);
		void returnUniqueID(void);

	private:
		static SCrossForgeDevice* m_pInstance; ///< Unique instance pointer.
		static int16_t m_InstanceCount; ///< Number of instantiation calls

		class SLogger* m_pLogger; ///< Logger instance.
		class SGPIO* m_pGPIO; ///< GPIO instance
		class SAssetIO* m_pAssIO; ///< Asset importer/exporter instance
		class STextureManager* m_pTexMan; ///< Texture manager
		class SShaderManager* m_pSMan;	///< Shader manager

		std::vector<CForgeObject*> m_RegisteredObjects;
		std::list<uint32_t> m_FreeObjSlots;
		std::mutex m_Mutex;

	};//SCrossForgeDevice
}//name-space