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

#include <CForge/Core/CForgeObject.h>
#include "FontFace.h"

namespace CForge {

	class SFontManager : public CForgeObject {
	public:
		static SFontManager* instance(void);
		void release(void);

		FontFace* createFont(std::string Filename, uint32_t Size, Eigen::Vector4f Color);
		FontFace* createFont(FontFace::FontStyle Style);
		void releaseFont(FontFace* pFont);

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
			FontFace* pInstance;
			FontFace::FontStyle StyleParams;
			uint32_t InstanceCount;
		};

		std::vector<FontInstance*> m_FontInstances;

		void *m_pFTLib;
		
	};//SFontManager

	typedef SFontManager FontManager;

}//name-space

#endif 