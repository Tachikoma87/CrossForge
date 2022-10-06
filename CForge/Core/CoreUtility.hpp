/*****************************************************************************\
*                                                                           *
* File(s): SCoreUtility.hpp                                     *
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
#ifndef __CFORGE_SCOREUTILITY_H__
#define __CFORGE_SCOREUTILITY_H__

#include <inttypes.h>
#include <math.h>
#include <chrono>
#include <limits>
#include <Eigen/Eigen>
#include "CrossForgeException.h"
#include "CoreDefinitions.h"
#include "../AssetIO/File.h"

namespace CForge {
	/**
	* \brief Provides basic utility functions.
	* 
	* \todo Do full documentation
	* \todo Add timestamp method
	*/
	class CFORGE_API CoreUtility {
	public:
		CoreUtility(void) {

		}//Constructor

		~CoreUtility(void) {

		}//Destructor

		 template<typename T>
		 static void memset(T* pMemory, T Value, uint32_t Count) {
			for (uint32_t i = 0; i < Count; ++i) pMemory[i] = Value;
		}//memset


		template<typename T>
		static T randRange(T Lower, T Upper) {
			long double R = (long double)(rand()) / (long double)(randMax());
			long double Temp = R * (Upper - Lower);
			return T(Lower + Temp);
			//return T(Lower + R * (Upper - Lower));
		}//randRange

		static uint64_t rand(void);
		static void randSeed(uint64_t Seed); 

		static uint64_t randMax(void) {
			return std::numeric_limits<uint64_t>::max()/2ull;
		}//randMax

		static uint64_t timestamp(void) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}//timestamp

		static std::string toLowerCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::tolower(Rval[i]);
			return Rval;
		}//toLowerCase

		static std::string toUpperCase(const std::string Str) {
			std::string Rval = Str;
			for (uint32_t i = 0; i < Rval.length(); ++i) Rval[i] = std::toupper(Rval[i]);
			return Rval;
		}//toUpperCase

	protected:
		static uint64_t m_RndState; // defined in SCrossForgeDevice.cpp
	};

}//name space

#endif