/*****************************************************************************\
*                                                                           *
* File(s): ITCaller.hpp                        *
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
#ifndef __CFORGE_ITCALLER_HPP__
#define __CFORGE_ITCALLER_HPP__

#include "CForgeObject.h"
#include "ITListener.hpp"

namespace CForge {

	/**
	* \brief Plug-in for a class that provides callback functionality.
	*
	* By inheriting this class other classes can register to listen and the inheriting class can set broadcast messages to all listeners.
	* \todo Do full documentation.
	*/
	template<typename T>
	class ITCaller {
	public:
		void startListening(ITListener<T>* pListener) {
			if (nullptr == pListener) throw NullpointerExcept("pListener");
			
			int32_t Index = -1;
			for (int32_t i = 0; i < m_Listeners.size(); ++i) {
				if (m_Listeners[i] == pListener) return; // 
				if (nullptr == m_Listeners[i]) Index = i;
			}//for[listeners]

			if (Index == -1) {
				Index = m_Listeners.size();
				m_Listeners.push_back(nullptr);
			}
			m_Listeners[Index] = pListener;

		}//startListening

		void stopListening(ITListener<T>* pListener) {
			for (auto& i : m_Listeners) {
				if (i == pListener) {
					i = nullptr;
					break;
				}
			}//for[all listeners]
		}//stopListening


	protected:
		ITCaller(void) {

		}

		~ITCaller(void) {
			m_Listeners.clear();
		}

		void broadcast(T Msg) {
			for (auto i : m_Listeners) {
				if (nullptr != i) i->listen(Msg);
			}//for[listeners]
		}//broadcast

		std::vector<ITListener<T>*> m_Listeners;
	};//ITCaller

}//name space

#endif 