#include <GLFW/glfw3.h>
#include "Mouse.h"
#include "../Core/CoreUtility.hpp"

using namespace Eigen;

namespace CForge {

	Mouse::Mouse(void): CForgeObject("Mouse") {
		m_Position = Vector2f(0.0f, 0.0f);
		m_Movement = Vector2f(0.0f, 0.0f);
		m_Wheel = Vector2f(0.0f, 0.0f);
		m_pWin = nullptr;
		CoreUtility::memset(&m_BtnState[0], false, BTN_COUNT);
	}//Constructor

	Mouse::~Mouse(void) {
		clear();
	}//Destructor

	void Mouse::init(GLFWwindow* pWin) {
		clear();
		if (nullptr == pWin) throw NullpointerExcept("pWin");
		m_pWin = pWin;
	}//initialize

	void Mouse::clear(void) {
		m_pWin = nullptr;
		m_Position = Vector2f(0.0f, 0.0f);
		m_Movement = Vector2f(0.0f, 0.0f);
		m_Wheel = Vector2f(0.0f, 0.0f);
		CoreUtility::memset(&m_BtnState[0], false, BTN_COUNT);
	}//clear

	Eigen::Vector2f Mouse::position(void)const {
		return m_Position;
	}//position

	Eigen::Vector2f Mouse::movement(void) const{
		return m_Movement;
	}//movement

	Eigen::Vector2f Mouse::wheel(void)const {
		return m_Wheel;
	}//wheel

	bool Mouse::buttonState(Button Btn)const {
		if (Btn <= BTN_UNKNOWN || Btn >= BTN_COUNT) throw IndexOutOfBoundsExcept("Btn");
		return m_BtnState[Btn];
	}//buttonState

	void Mouse::position(Eigen::Vector2f Pos) {
		m_Movement += Pos - m_Position;
		m_Position = Pos;
	}//position

	void Mouse::movement(Eigen::Vector2f Movement) {
		m_Movement = Movement;
	}//movement

	void Mouse::wheel(Eigen::Vector2f Offset) {
		m_Wheel = Offset;
	}//wheel

	void Mouse::buttonState(Button Btn, bool State) {
		if (Btn <= BTN_UNKNOWN || Btn >= BTN_COUNT) throw IndexOutOfBoundsExcept("Btn");
		m_BtnState[Btn] = State;
	}//buttonState

}//name-space