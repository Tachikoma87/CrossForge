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

		static std::vector<std::string> splitString(const std::string input, char delimiter = '\n') {
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

		static uint64_t timestampHighPrecision(void) {
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}//timestamp

		/**
		* \brief Converts a u32 string to a regular string.
		*
		* \param[in] String u32 input string.
		* \return Converted string.
		*/
		static std::string convertToString(const std::u32string String);

		/**
		* \brief Converts a wide string to a regular one.
		*
		* \param[in] String Input wide string.
		* \return Converted string.
		*/
		static std::string convertToString(const std::wstring String);

		/**
		* \brief Converts a regular string to a u32 string.
		*
		* \param[in] String The input string.
		* \return Converted u32 string.
		*/
		static std::u32string convertTou32String(const std::string String);

		/**
		* \brief Converts a regular string to a wide string.
		*
		* \param[in] String Input string to convert.
		* \return Converted string.
		*/
		static std::wstring convertToWString(const std::string String);

		/**
		* \brief Converts an RGB color vector to a grayscale value. Values have to be \f$ \in [0,1] \f$.
		*
		* \param[in] C RGB color vector.
		* \return Grayscale value.
		*/
		static float rgbToGrayscale(const Eigen::Vector3f C) {
			return rgbToGrayscale(C.x(), C.y(), C.z());
		}

		/**
		* \brief Converts RGB values into grayscale representation. Values have to be \$f \in [0,1] \$f.
		*
		* \param[in] R Red value.
		* \param[in] G Green value.
		* \param[in] B Blue value.
		* \return Grayscale value.
		*/
		static float rgbToGrayscale(float R, float G, float B) {
			return 0.299 * R + 0.587 * G + 0.114 * B;
		}

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
