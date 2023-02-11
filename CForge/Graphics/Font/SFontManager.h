/*****************************************************************************\
*                                                                           *
* File(s): SFontManager.h and SFontManager.cpp                                            *
*                                                                           *
* Content: Font manager singleton.   *
*                        *
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
#ifndef __CFORGE_SFONTMANAGER_H__
#define __CFORGE_SFONTMANAGER_H__

#include "../../Core/CForgeObject.h"
#include "Font.h"

namespace CForge {

	class CFORGE_API SFontManager : public CForgeObject {
	public:
		static SFontManager* instance(void);
		void release(void);

		Font* createFont(std::string Filename, uint32_t Size);
		Font* createFont(Font::FontStyle Style);
		void releaseFont(Font* pFont);

		void* freetypeLibraryHandle(void);

	protected:
		SFontManager(void);
		~SFontManager(void);

		void init(void);
		void clear(void);

	private:
		static uint32_t m_InstanceCount;
		static SFontManager* m_pInstance;

		struct FontInstance {
			Font* pInstance;
			Font::FontStyle StyleParams;
			uint32_t InstanceCount;
		};

		std::vector<FontInstance*> m_FontInstances;

		void *m_pFTLib;
		
	};//SFontManager

	typedef SFontManager FontManager;

}//name-space

#endif 