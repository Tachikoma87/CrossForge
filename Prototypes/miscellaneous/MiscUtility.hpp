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

#include <crossforge/GlobalDefinitions.h>
#include <iomanip>

namespace crossforge {
	class MiscUtility {
	public:
		static std::string getTimeISO(const uint64_t timestamp) {
			std::shared_ptr<char[]> pBuffer = std::shared_ptr<char[]>(new char[128]);

			uint16_t ms = (timestamp % 1000UL);

			try {
				const std::time_t point = (const std::time_t)(timestamp / 1000);
				strftime(pBuffer.get(), 128, "%F %T", std::gmtime(&point));
			}
			catch (const std::exception& e) {
				LogError("Exception during time cast: " + std::string(e.what()));
			}

			std::string rval = std::string(pBuffer.get()) + ".";
			if (ms < 10) rval += "00";
			else if (ms < 100) rval += "0";
			return rval + std::to_string(ms);
		}

		static uint64_t getTimestampFromIsoTime(const std::string timestring) {
			std::time_t rval;

			std::tm tm = {};
			std::istringstream ss(timestring);

			ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
			rval = mktime(&tm);

			uint64_t ms = 0;
			const uint64_t dotPos = timestring.find(".");
			if (dotPos != std::string::npos) {
				std::string msString = timestring.substr(dotPos + 1).c_str();
				if (msString.length() == 1) msString += "00";
				else if (msString.length() == 2) msString += "0";
				sscanf(msString.c_str(), "%I64d", &ms);
			}
			if (ms > 1000) ms = 0;

			return rval * 1000 + ms;
		}


		/**
		* \brief Defines for available default fonts.
		*/
		enum DefaultFontType : int8_t {
			FONTTYPE_UNKNOWN = -1,	///< Default value.
			FONTTYPE_SANSERIF = 0,	///< Font without serifs.
			FONTTYPE_SERIF,			///< Font with serif.
			FONTTYPE_MONO,			///< Mono font.
			FONTTYPE_HANDWRITING,	///< Handwriting font.

			FONTTYPE_COUNT			///< Number of default fonts.
		};//DefaultFont
		/**
		* \brief Creates a default font with the specified traits.
		*
		* \param[in] FontType Type of the font.
		* \param[in] FontSize Size of the font.
		* \param[in] Bold Whether font should appear in bold style.
		* \param[in] Italic Whether font should appear in italic style.
		* \param[in] CharSet The character set to use. If empty the default character set will be used. \see Font::FontStyle
		* \return Font handle.
		* @ToDo: Do later
		*/
		//static Font* defaultFont(DefaultFontType FontType, uint32_t FontSize, bool Bold = false, bool Italic = false, std::u32string CharSet = U"");

	protected:
		MiscUtility(){}
		~MiscUtility(){}
	};
}

#endif 
