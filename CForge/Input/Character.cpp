#include <GLFW/glfw3.h>
#include "../Core/CoreUtility.hpp"
#include "Character.h"

namespace CForge {


	Character::Character(void): CForgeObject("Character") {
		m_pWin = nullptr;
	}//Constructor

	Character::~Character(void) {
		clear();
	}//Destructor

	void Character::init(GLFWwindow* pWin) {
		clear();
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		m_pWin = pWin;
	}//initialize

	void Character::clear(void) {
		m_pWin = nullptr;
		m_Listeners.clear();
	}//clear

	void Character::sendChar(unsigned int codepoint) {
		broadcast((char32_t)codepoint);
	}

}//name-space
