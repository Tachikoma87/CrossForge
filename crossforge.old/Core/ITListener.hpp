/*****************************************************************************\
*                                                                           *
* File(s): ITListener.hpp                                                   *
*                                                                           *
* Content: Listener interface for the caller/listener plug-in principle.    *
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
#ifndef __CFORGE_ITLISTENER_HPP__
#define __CFORGE_ITLISTENER_HPP__


#include "CForgeObject.h"

namespace CForge {
	/**
	* \brief Plug-in that works together with ITCaller. By implementing the listen function, messages can be received.
	* \see CForge::ITCaller
	*
	*/
	template<typename T>
	class ITListener {
	public:
		/**
		* \brief This method has to be implemented in order to receive messages.
		* \param[in] Msg Message to receive and process.
		*/
		virtual void listen(const T Msg) = 0;

	protected:
		/**
		* \brief Constructor
		*/
		ITListener(void) {

		}

		/**
		* \brief Destructor
		*/
		virtual ~ITListener(void) {

		}
	private:

	};//ITListener

}//name space


#endif 