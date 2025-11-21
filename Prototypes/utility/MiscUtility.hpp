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
