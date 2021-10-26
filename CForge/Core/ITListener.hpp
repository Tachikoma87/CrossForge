/*****************************************************************************\
*                                                                           *
* File(s): ITListener.hpp                        *
*                                                                           *
* Content:    *
*          .                                         *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
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
	*
	* \todo Do full documentation.
	*/
	template<typename T>
	class ITListener {
	public:
		virtual void listen(const T Msg) = 0;

	protected:
		ITListener(void) {

		}

		virtual ~ITListener(void) {

		}
	private:

	};//ITListener

}//name space


#endif 