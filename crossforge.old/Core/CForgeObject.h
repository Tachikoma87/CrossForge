/*****************************************************************************\
*                                                                           *
* File(s): CForgeObject.h and CForgeObject.cpp                              *
*                                                                           *
* Content: Base class for all objects belonging to CrossForge.              *
*                                                                           *
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
#ifndef __CFORGE_CFORGEOBJECT_H__
#define __CFORGE_CFORGEOBJECT_H__


#include <Eigen/Eigen>
#include <vector>
#include <string>
#include "CoreDefinitions.h"

namespace CForge {
	/**
	* \brief Base class of all engine objects. Provides an object identifier and the class's name.
	* 
	* \ingroup Core
	* 
	*/
	class CFORGE_API CForgeObject {
	public:
		/**
		* \brief Special IDs.
		*/
		enum SpecialIDs : uint64_t {
			INVALID_ID = 0xFFFFFFFFFFFFFFFF,	///< Invalid ID used for initialization.
		};

		/**
		* \brief Getter for object ID.
		* \return Object identifier.
		*/
		uint64_t objectID()const;

		/**
		* \brief Getter for class name.
		* \return Class name.
		*/
		std::string className()const;

	protected:
		
		/**
		* \brief Constructor.
		* \param[in] The class name of the parent class.
		*/
		CForgeObject(const std::string ClassName);

		/**
		* \brief Copy constructor.
		* \param[in] other The other object.
		*/
		CForgeObject(const CForgeObject& other);

		/**
		* \brief Copy constructor.
		* \param[in] other The other object.
		*/
		CForgeObject(CForgeObject& other);

		/**
		* \brief Setter for the class name.
		* \param[in] ClassName New class name.
		*/
		void className(const std::string ClassName);

		/**
		* \brief Destructor.
		*/
		virtual ~CForgeObject();

		uint64_t m_ObjectID;	///< Unique ID of the object.
		std::string m_ClassName;///< Class name of derived class.

		static uint64_t m_ObjCounter;	///< Static counter to assign unique IDs.
	};//CForgeObject

	typedef std::shared_ptr<CForgeObject> CForgeObjectPtr;	///< Smart pointer definition.

	class SLogger;

}//name-space

#include "CrossForgeException.h"

#endif 