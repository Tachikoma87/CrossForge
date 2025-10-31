/*****************************************************************************\
*                                                                           *
* File(s): ITCaller.hpp                                                     *
*                                                                           *
* Content: Caller interface of the caller/listener plugin model.            *
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
#ifndef __CFORGE_ITCALLER_HPP__
#define __CFORGE_ITCALLER_HPP__

#include "CForgeObject.h"
#include "ITListener.hpp"

namespace CForge {

	/**
	* \brief Plug-in for a class that provides callback functionality.
	*
	* By inheriting this class, other classes can register to listen and the inheriting class can send broadcast messages to all listeners.
	* 
	* \todo Write tutorial page on the call/listener plug-in principle.
	*/
	template<typename T>
	class ITCaller {
	public:

		/**
		* \brief Registers a new listener, i.e. adds the specified class to the list of message receivers.
		* \param[in] pListener Class to register.
		* 
		* \remarks The class is ignored if it is already registered.
		*/
		void startListening(ITListener<T>* pListener) {
			if (nullptr == pListener) throw NullpointerExcept("pListener");
			
			int32_t Index = -1;
			for (int32_t i = 0; i < m_Listeners.size(); ++i) {
				if (m_Listeners[i] == pListener) return; // We already know this listener
				if (nullptr == m_Listeners[i]) {
					// Found a free spot.
					Index = i;
					break;
				}
			}//for[listeners]

			if (Index == -1) {
				Index = m_Listeners.size();
				m_Listeners.push_back(nullptr);
			}
			m_Listeners[Index] = pListener;

		}//startListening

		/**
		* \brief Removes a class from the list of message receivers.
		* \param[in] Class to dismiss.
		*/
		void stopListening(const ITListener<T>* pListener) {
			for (auto& i : m_Listeners) {
				if (i == pListener) {
					i = nullptr;
					break;
				}
			}//for[all listeners]
		}//stopListening

		/**
		* \brief Checks whether a class is already listening or not.
		* \param[in] pListener Class to check.
		* \return True if the class is listening, false otherwise.
		*/
		bool isListening(const ITListener<T>* pListener) const {
			bool Rval = false;
			for (auto i : m_Listeners) {
				if (i == pListener) {
					Rval = true;
					break;
				}
			}//for[all listeners]
			return Rval;
		}//isListening

	protected:
		/**
		* \brief Constructor
		*/
		ITCaller(void) {

		}

		/**
		* \brief Destructor
		*/
		~ITCaller(void) {
			m_Listeners.clear();
		}

		/**
		* \brief Broadcasts a message to all listeners.
		* \param[in] Msg Message to broadcast.
		*/
		void broadcast(const T Msg) {
			for (auto i : m_Listeners) {
				if (nullptr != i) i->listen(Msg);
			}//for[listeners]
		}//broadcast

		std::vector<ITListener<T>*> m_Listeners;	///< Registered listeners.
	};//ITCaller

}//name space

#endif 