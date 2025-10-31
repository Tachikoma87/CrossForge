/*****************************************************************************\
*                                                                           *
* File(s): MiscUtility.hpp                        *
*                                                                           *
* Content:                            *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tachikoma87                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* MIT License without any warranty or guaranty to work properly.            *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/
#ifndef __CROSSFORGE_MISCUTILITY_HPP__
#define __CROSSFORGE_MISCUTILITY_HPP__

#include <crossforge/core/CoreDefinitions.h>

namespace crossforge {
	class MiscUtility {
	public:
		static std::vector<std::string> splitIntoLines(const std::string input, char delimiter = '\n') {
			std::vector<std::string> result;
			std::stringstream stream(input);
			std::string line;
			try {
				while (std::getline(stream, line, delimiter)) result.push_back(line);
			}
			catch (const std::exception& e) {
				LogError("Exception occurred splitting string: " + std::string(e.what()));
			}
			catch (...) {
				LogError("Some not handled exception occurred during splitting string.");
			}
			
			return result;
		}
	protected:
		MiscUtility(){}
		~MiscUtility(){}
	};
}

#endif 
