//
// Header that gets included from every *.cpp file
// It includes frequently used system headers and provides some helper functions
//

#pragma once

#include <windows.h>
#include <tchar.h>

#include <cassert>
#ifdef _DEBUG
#define verify(e) assert(e)
#else
#define verify(e) ;e;
#endif

#include <string>
#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#include <sstream>
#ifdef _UNICODE
typedef std::wstringstream tstringstream;
#else
typedef std::stringstream tstringstream;
#endif

// Display context menu at the cursor
//
// According to the requirements of WinAPI function TrackPopupMenu,
// the menu passed must contain the only submenu, that will get displayed in turn
// 
// menu - a menu (see comment above)
// window - window that will receive WM_COMMANDs from the context menu
//
void showContextMenu(HMENU menu, HWND window);

//
// Source: http://www.rsdn.ru/article/ui/multilang.xml
//
HGLOBAL LoadResourceLang(LPCTSTR resType, LPCTSTR resource);

//
// Source: http://www.rsdn.ru/article/ui/multilang.xml
//
tstring LoadStringLang(UINT strID);
