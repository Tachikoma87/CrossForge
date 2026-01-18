#include "MiscUtility.hpp"

namespace crossforge {

	// @ToDo: Do later
	//Font* CForgeUtility::defaultFont(DefaultFontType FontType, uint32_t FontSize, bool Bold, bool Italic, std::u32string CharSet) {

	//	Font::FontStyle Style;
	//	switch (FontType) {
	//	case FONTTYPE_SERIF: {
	//		if (Bold && Italic) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-BoldItalic.ttf";
	//		else if (Bold) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Bold.ttf";
	//		else if (Italic) Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Italic.ttf";
	//		else Style.FileName = "Assets/Fonts/NotoSerif/NotoSerif-Regular.ttf";
	//	}break;
	//	case FONTTYPE_SANSERIF: {
	//		if (Bold && Italic) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-SemiBoldItalic.ttf";
	//		else if (Bold) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-SemiBold.ttf";
	//		else if (Italic) Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-Italic.ttf";
	//		else Style.FileName = "Assets/Fonts/SourceSansPro/SourceSansPro-Regular.ttf";
	//	}break;
	//	case FONTTYPE_MONO: {
	//		if (Bold && Italic) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-BoldItalic.ttf";
	//		else if (Bold) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Bold.ttf";
	//		else if (Italic) Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Italic.ttf";
	//		else Style.FileName = "Assets/Fonts/UbuntuMono/UbuntuMono-Regular.ttf";
	//	}break;
	//	case FONTTYPE_HANDWRITING: {
	//		// for handwriting we have to varying type faces
	//		Style.FileName = "Assets/Fonts/RougeScript/RougeScript-Regular.ttf";
	//	}break;
	//	default: throw CForgeExcept("Unknown font type specified!");
	//	}

	//	Style.PixelSize = FontSize;
	//	if (!CharSet.empty()) Style.CharacterSet = CharSet;

	//	FontManager* pFMan = FontManager::instance();
	//	Font* pRval = pFMan->createFont(Style);
	//	pFMan->release();
	//	return pRval;
	//}//defaultFont
}