//
// Заголовочный файл, который включается во все файлы с исходным кодом
// Подключает часто используемые заголовочные файлы и определяет вспомогательные функци.
//

#pragma once

#include <windows.h>
#include <tchar.h>

// Отобразить контекстное меню в том месте, где сейчас находится курсор мыши.
// Из-за ограничений функции TrackPopupMenu переданное меню должно содержать
// единственное подменю, которое собственно и будет отображено
// 
// menu - меню, см. комментарий выше
// window - окно, которому при выборе пункта меню придёт сообщение WM_COMMAND
//
void showContextMenu(HMENU menu, HWND window);

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
