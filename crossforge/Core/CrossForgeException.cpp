#include "CrossForgeException.h"

using namespace std;

namespace CForge {

	CrossForgeException::CrossForgeException(Type T, const std::string Message, const std::string Info01, const std::string Info02, const std::string File, const int32_t Line, const std::string Function): CForgeObject("CrossForgeException") {
		m_Message = Message;
		m_File = File;
		m_Line = Line;
		m_Function = Function;
		m_Type = T;
		m_Info01 = Info01;
		m_Info02 = Info02;
	}//Constructor

	CrossForgeException::~CrossForgeException(void) {
		m_Message = "";
	}//Destructor

	std::string CrossForgeException::msg(void)const {
		return m_Message;
	}//message

	int32_t CrossForgeException::line(void)const {
		return m_Line;
	}//line

	string CrossForgeException::file(void)const {
		return m_File;
	}//file

	string CrossForgeException::function(void)const {
		return m_Function;
	}//function

	CrossForgeException::Type CrossForgeException::type(void)const {
		return m_Type;
	}//type

	std::string CrossForgeException::info01(void)const {
		return m_Info01;
	}//info01

	std::string CrossForgeException::info02(void)const {
		return m_Info02;
	}//info02




	void CrossForgeException::msg(const std::string Msg) {
		m_Message = Msg;
	}//message

	void CrossForgeException::line(const int32_t Line) {
		m_Line = Line;
	}//line

	void CrossForgeException::file(const std::string File) {
		m_File = File;
	}//file

	void CrossForgeException::function(const std::string Function) {
		m_Function = Function;
	}//function

	void CrossForgeException::type(const Type T) {
		m_Type = T;
	}//type

	void CrossForgeException::info01(const std::string Info01) {
		m_Info01 = Info01;
	}//info01

	void CrossForgeException::info02(const std::string Info02) {
		m_Info02 = Info02;
	}//info02

}//PiForge