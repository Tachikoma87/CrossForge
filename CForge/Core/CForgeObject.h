/*****************************************************************************\
*                                                                           *
* File(s): CForgeObject.h and CForgeObject.cpp                              *
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
#ifndef __CFORGE_CFORGEOBJECT_H__
#define __CFORGE_CFORGEOBJECT_H__


#include <Eigen/Eigen>
#include <vector>
#include <string>
#include "CoreDefinitions.h"

namespace CForge {
	/**
	* \brief Base class of all engine objects. Provides an object identifier and the classe's name.
	* 
	* \todo Do full documentation
	* 
	*/
	class CFORGE_API CForgeObject {
	public:
		enum SpecialIDs : uint32_t {
			INVALID_ID = 0xFFFFFFFF,
		};

		uint32_t objectID(void)const;
		std::string className(void)const;

	protected:
		

		CForgeObject(const std::string ClassName);
		CForgeObject(const CForgeObject& other);
		virtual ~CForgeObject(void);

		uint32_t m_ObjectID;
		std::string m_ClassName;
	};//CForgeObject

	
	class SLogger;

}//name-space

#include "CrossForgeException.h"

#endif 