#include "SCrossForgeDevice.h"
#include "CForgeObject.h"

namespace CForge {
	uint64_t CForgeObject::m_ObjCounter = 0;

	CForgeObject::CForgeObject(const std::string ClassName) {
		m_ClassName = ClassName;		
		m_ObjectID = m_ObjCounter++;
	}//Constructor

	CForgeObject::CForgeObject(const CForgeObject& other) {
		m_ClassName = other.className();
		m_ObjectID = m_ObjCounter++;
	}//Constructor

	CForgeObject::CForgeObject(CForgeObject& other) {
		m_ClassName = other.className();
		m_ObjectID = other.objectID();
	}//Constructor

	CForgeObject::~CForgeObject() {
		m_ObjectID = INVALID_ID;
		m_ClassName = "";
	}//Destructor

	uint64_t CForgeObject::objectID()const {
		return m_ObjectID;
	}//objectID

	std::string CForgeObject::className()const {
		return m_ClassName;
	}//className

	void CForgeObject::className(const std::string ClassName) {
		m_ClassName = ClassName;
	}

}//name space