/*****************************************************************************\
*                                                                           *
* File(s): CrossForgeException.h and CrossForgeException.cpp                *
*                                                                           *
* Content: The basic exception thrown by the CrossForge library.            *
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
#pragma once

#include "CForgeObject.h"

namespace CForge {

	/**
	* \brief Basic exception thrown by the CrossForge library.
	*
	* \ingroup Core
	* \todo Write documentation concept page on exception handling.
	*/
	class CFORGE_API CrossForgeException: public CForgeObject {
	public:
		/**
		*\brief Defines exception types.
		*/
		enum struct Type: int32_t {
			UNKNOWN = -1,	///< An unknown exception without any type.
			GENERAL,		///< A general exception.
			NULLPOINTER,	///< Some variable or argument was a nullpointer without being allowed to.
			INDEX_OUT_OF_BOUNDS, ///< An invalid index was reached or specified.
			NOT_INITIALIZED, ///< A class was not properly initialized. Call init() or begin() with appropriate attributes first to use a class.
			OUT_OF_MEMORY, ///< memory allocation failed.
		};

		/**
		* \brief Constructor
		* \param[in] T Exception type. \see CrossForgeException::Type
		* \param[in] Message The general message to be logged.
		* \param[in] Info01 Additional information 1
		* \param[in] Info02 Additional information 2
		* \param[in] File The file in which the exception occurred. You can use __FILE__ for this one.
		* \param[in] Line Code line where the exception occurred. You can use __LINE__ for this one.
		* \param[in] Function Function in which the exception occurred. You can use __FUNCTION__ (Windows) or __PRETTY_FUNCTION__ (Linux) for this one. 
		*/
		CrossForgeException(Type T, const std::string Message, const std::string Info01, const std::string Info02, const std::string File, const int32_t Line, const std::string Function);

		/**
		* \brief Destructor
		*/
		~CrossForgeException(void);

		/**
		* \brief Message getter.
		* \return Exception message.
		*/
		std::string msg(void)const;

		/**
		* \brief Line getter.
		* \return Line where the exception was raised.
		*/
		int32_t line(void)const;

		/**
		* \brief File getter.
		* \return File in which the exception was raised.
		*/
		std::string file(void)const;

		/**
		* \brief Function getter.
		* \return Function where the exception was raised.
		*/
		std::string function(void)const;

		/**
		* \brief Type getter.
		* \return Exception type. \see CrossForgeException::Type
		*/
		Type type(void)const;

		/**
		* \brief Info01 getter.
		* \return Additional information 1 of this exception.
		*/
		std::string info01(void)const;

		/**
		* \brief Info02 getter.
		* \return Additional information 2 of this exception.
		*/
		std::string info02(void)const;

		/**
		* \brief Message setter.
		* \param[in] Msg New message attribute.
		*/
		void msg(const std::string Msg);

		/**
		* \brief Line setter.
		* \param[in] Line New line attribute.
		*/
		void line(const int32_t Line);

		/**
		* \brief File setter.
		* \param[in] File New file attribute.
		*/
		void file(const std::string File);

		/**
		* \brief Function setter.
		* \param[in] Function New function attribute.
		*/
		void function(const std::string Function);

		/**
		* \brief Type setter. 
		* \param[in] T New type attribute.
		* \see CrossForgeException::Type
		*/
		void type(const Type T);

		/**
		* \brief Info01 setter.
		* \param[in] Info01 New Info 1 attribute.
		*/
		void info01(const std::string Info01);

		/**
		* \brief Info02 setter.
		* \param[in] Info02 New Info 2 attribute.
		*/
		void info02(const std::string Info02);

	protected:
		std::string m_Message;		///< Message of this exception.
		std::string m_File;			///< File where the exception occurred.
		std::string m_Function;		///< Function/method in which the exception occurred.
		int32_t m_Line;				///< Code line where the exception occurred.
		Type m_Type;				///< Type of the exception. 

		std::string m_Info01;		///< Additional information 1.
		std::string m_Info02;		///< Additional information 2.

	};//CPiForgeException

	

#ifdef WIN32
#define CForgeExcept(Msg) CForge::CrossForgeException(CForge::CrossForgeException::Type::GENERAL, Msg, "", "", __FILE__, __LINE__, __FUNCTION__)
#define NullpointerExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::NULLPOINTER, Variable, "", "", __FILE__, __LINE__, __FUNCTION__)
#define NotInitializedExcept(What) CForge::CrossForgeException(CForge::CrossForgeException::Type::NOT_INITIALIZED, What, "", "", __FILE__, __LINE__, __FUNCTION__)
#define IndexOutOfBoundsExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::INDEX_OUT_OF_BOUNDS, Variable, "", "", __FILE__,__LINE__, __FUNCTION__)
#define OutOfMemoryExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::OUT_OF_MEMORY, Variable, "", "", __FILE__, __LINE__, __FUNCTION__)
#else
#define CForgeExcept(Msg) CForge::CrossForgeException(CForge::CrossForgeException::Type::GENERAL, Msg, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define NullpointerExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::NULLPOINTER, Variable, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define NotInitializedExcept(What) CForge::CrossForgeException(CForge::CrossForgeException::Type::NOT_INITIALIZED, What, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define IndexOutOfBoundsExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::INDEX_OUT_OF_BOUNDS, Variable, "", "", __FILE__,__LINE__, __PRETTY_FUNCTION__)
#define OutOfMemoryExcept(Variable) CForge::CrossForgeException(CForge::CrossForgeException::Type::OUT_OF_MEMORY, Variable, "", "", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

}//name-space
