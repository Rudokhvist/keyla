#include "common.h"
#include "res/resource.h"

void showContextMenu(HMENU menu, HWND window) {
	// This is necessary to make the menu disappear when user clicks outside of it
	SetForegroundWindow(window);

	POINT pt;
	verify(GetCursorPos(&pt));

	verify(TrackPopupMenu(GetSubMenu(menu, 0), 0, pt.x, pt.y, 0, window, 0));
}

HGLOBAL LoadResourceLang(LPCTSTR resType, LPCTSTR resource)
{
  HINSTANCE hAppInstance=GetModuleHandle(NULL);

  HRSRC hrRes=FindResourceEx(hAppInstance, resType, resource, GetUserDefaultUILanguage());
  if (!hrRes)
  {
    hrRes=FindResourceEx(hAppInstance, resType, resource, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
  }

  return LoadResource(hAppInstance, hrRes);
}

tstring LoadStringLang(UINT strID)
{
  LPCWSTR str=(LPCWSTR)LoadResourceLang(RT_STRING, MAKEINTRESOURCE(1+(strID >> 4)));
  assert(str);
  if (!str)
    return tstring();

  for (WORD strPos=0; strPos < (strID & 0x000F); strPos++)
    str+=*str+1;

#ifdef _UNICODE
  return tstring(str+1, *str);
#else
  WORD len = *str;
  vector<TCHAR> buf(len + 1);
  WideCharToMultiByte(CP_ACP, 0, str+1, *str+1, &buf[0], len, NULL, NULL);

  buf[len] = 0;
  return tstring(&buf[0]);
#endif
}
