#include "common.h"
#include "layoutList.h"

namespace layoutList {

	std::vector<HKL> GuiLayoutList;

	void load() {
		unsigned int layoutsCount = GetKeyboardLayoutList(0, 0);
		GuiLayoutList.resize(layoutsCount);
		GetKeyboardLayoutList(layoutsCount, &GuiLayoutList[0]);
	}

	tstring layoutLangId(HKL layout) {
		TCHAR buf[6];
		_itot(LOWORD(layout), buf, 16);
		return tstring(buf);
	}
	
	tstring layoutLanguage(HKL layout) {
		const unsigned int langNameBufferSize = 256;
		TCHAR langNameBuffer[langNameBufferSize];

		LANGID langid = LOWORD(layout);
		LCID localeid = MAKELCID(langid, SORT_DEFAULT);
		GetLocaleInfo(localeid, LOCALE_SLANGUAGE, langNameBuffer, langNameBufferSize);
		return tstring(langNameBuffer);
	}

}