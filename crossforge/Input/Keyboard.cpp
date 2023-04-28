#include <GLFW/glfw3.h>
#include "../Utility/CForgeUtility.h"
#include "Keyboard.h"

namespace CForge {


	Keyboard::Keyboard(void): CForgeObject("Keyboard") {
		m_pWin = nullptr;
		CForgeUtility::memset(&m_KeyStates[0], KEY_RELEASED, KEY_COUNT);
	}//Constructor

	Keyboard::~Keyboard(void) {
		clear();
	}//Destructor

	void Keyboard::init(GLFWwindow* pWin) {
		clear();
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		m_pWin = pWin;
	}//initialize

	void Keyboard::clear(void) {
		m_pWin = nullptr;
		CForgeUtility::memset(&m_KeyStates[0], KEY_RELEASED, KEY_COUNT);
	}//clear

	bool Keyboard::keyPressed(Key K)const {
		if (K <= KEY_UNKNOWN || K >= KEY_COUNT) throw IndexOutOfBoundsExcept("K");
		return (m_KeyStates[K] != KEY_RELEASED);
	}//keyPressed

	bool Keyboard::keyPressed(Key K, bool Reset) {
		if (K <= KEY_UNKNOWN || K >= KEY_COUNT) throw IndexOutOfBoundsExcept("K");
		const bool Rval = (m_KeyStates[K] != KEY_RELEASED);
		if (Reset) m_KeyStates[K] = KEY_RELEASED;
		return Rval;
	}//keyPressed

	bool Keyboard::keyPressed(Key K1, Key K2, Key K3)const {
		bool Rval = keyPressed(K1);
		Rval &= keyPressed(K2);
		if (K3 != KEY_UNKNOWN) Rval &= keyPressed(K3);
		return Rval;
	}//keyPressed

	void Keyboard::keyState(Key K, State S) {
		if (K <= KEY_UNKNOWN || K >= KEY_COUNT) throw IndexOutOfBoundsExcept("K");
		m_KeyStates[K] = S;
		KeyboardCallback broadcastObj;
		broadcastObj.key = K;
		broadcastObj.state = S;
		broadcastObj.Unicode = 0;
		broadcast(broadcastObj);
	}//keyPressed

	Keyboard::State Keyboard::keyState(Key K)const {
		if (K <= KEY_UNKNOWN || K >= KEY_COUNT) throw IndexOutOfBoundsExcept("K");
		return m_KeyStates[K];
	}//


	void Keyboard::textInput(uint32_t Character) {
		KeyboardCallback broadcastObj;
		broadcastObj.key = KEY_UNKNOWN;
		broadcastObj.state = KEY_PRESSED;
		broadcastObj.Unicode = Character;
		broadcast(broadcastObj);
	}//textInput

}//name-space
