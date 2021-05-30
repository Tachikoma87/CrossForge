/*****************************************************************************\
*                                                                           *
* File(s): SUtilities.hpp                                  *
*                                                                           *
* Content:    *
*                                                   *
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

#ifndef __CFORGE_SUTILITIES_HPP__
#define __CFORGE_SUTILITIES_HPP__

#include <inttypes.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

class Utilities {
public:

	static uint64_t timestampMilliseconds(void){
#ifdef WIN32
		return GetTickCount64();
#else
		struct timeval tv;
		clock_gettime(CLOCK_MONOTONIC, &tv)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
	}//retriveUpTime


protected:
	Utilities(void) {

	}

	~Utilities(void) {

	}
};//Utilities

#endif //Header Guard