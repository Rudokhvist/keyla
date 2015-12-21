#include "common.h"
#include "application.h"
#include "layoutList.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "settings.h"
#include "settingsWindow.h"
#include "trayIcon.h"
#include "strsafe.h"
#include <algorithm>
#include <map>

#ifdef _DEBUG
#define ICONBGCOLOR RGB(0,128,128)
#else
#define ICONBGCOLOR RGB(0,0,128)
#endif

using namespace std;

namespace {

// Message identifier used by the tray icon
const unsigned int TrayIconMessage = WM_USER + 1;

// Identifier of the only tray icon 
const unsigned int TrayIconId = 0;

// Tray icon's tooltip
const tstring Tooltip = LoadStringLang(IDS_KEYLA_IS_A_KEYBOARD_LAYOUT_SWITCHER);


// Tray icon menu
HMENU Menu = 0;

// Main window's HWND. Passed through create()
HWND Window = 0;


// Here is a map of icons corresponding to particular layouts
// key - layout's language id as string
// value - icon's handle (HICON)
class TLayoutIcons : public map<pair<tstring, bool>, HICON> {
public:

	using map<pair<tstring, bool>, HICON>::find;
	iterator find(const tstring & langid) {
		return find(make_pair(langid, Application::GetApp()->isActive()));
	}

	using map<pair<tstring, bool>, HICON>::insert;
	pair<iterator, bool> insert(const tstring & langid, HICON icon) {
		return insert(make_pair(make_pair(langid, Application::GetApp()->isActive()), icon));
	}
} LayoutIcons; 


bool messageHandler(HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT * ret) {

	// Message from the tray icon
	if (message == TrayIconMessage && wparam == TrayIconId)
		// NOTE: Do not allow to touch the icon when the settings window is shown
		switch (lparam) {
			case WM_CONTEXTMENU:
			case WM_RBUTTONUP:
				if (settingsWindow::isShown()) 
					MessageBeep(-1);
				else
					trayIcon::showMenu();
				return true;
			case NIN_KEYSELECT:
			case NIN_SELECT:
			case WM_LBUTTONUP:
				if (settingsWindow::isShown()) 
					MessageBeep(-1);
				else
					settingsWindow::show();
				return true;
		}
	else if (message == WM_COMMAND) {
		unsigned int code = HIWORD(wparam);
		unsigned int id = LOWORD(wparam);
		HWND hwnd = reinterpret_cast<HWND>(lparam);

		// Message from the context menu of the tray icon
		if (hwnd == 0 && code == 0)
			switch (id) {
				case ID_TRAYICONMENU_TOGGLE: {
					Application::GetApp()->toggle();

					tstring _1;
					if (Application::GetApp()->isActive())
						_1 = LoadStringLang(IDS_DISABLE);
					else
						_1 = LoadStringLang(IDS_ENABLE);
					MENUITEMINFO mii = {sizeof(mii), MIIM_TYPE, MFT_STRING};
					mii.dwTypeData = const_cast<LPTSTR>(_1.c_str());
					SetMenuItemInfo(trayIcon::getMenu(), id, FALSE, &mii);

					return 0;
				}
				case ID_TRAYICONMENU_SETTINGS:
					settingsWindow::show();
					return 0;
				case ID_TRAYICONMENU_EXIT:
					trayIcon::destroy();
					mainWindow::destroy();
					return 0;
			}
	}
	return false;
}

}

namespace trayIcon {

	void create(HWND mainWindow) {
		Window = mainWindow;

		// Add our message handler
		mainWindow::addMessageHandler(messageHandler);

		// Load the context menu from the resources
		Menu = LoadMenuIndirect(LoadResourceLang(RT_MENU, MAKEINTRESOURCE(IDM_TRAYICONMENU)));
		assert(Menu != 0);

		// Add the tray icon to the tray.
		//
		// Do not set an image here in order to eliminete flickering.
		// We will always do it in indicateLayout()
		//
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = TrayIconId;
		nid.uFlags = NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = TrayIconMessage;
		_tcsncpy(nid.szTip, Tooltip.c_str(), sizeof(nid.szTip) / sizeof(TCHAR));
		verify(Shell_NotifyIcon(NIM_ADD, &nid));
	}

	void showMenu() {
		showContextMenu(Menu, Window);
	}

	HICON CreateLangIcon(HKL Lang)
		//_In  Lang - Keybord layout
		//Result - icon with two-character lang name

	{
		wchar_t LangShort[10];
		LOGFONT lf;
		HFONT hf;
		HDC bdc;
		static HDC mdc = 0;
		HBITMAP LangBitmap;
		HBRUSH hBrush;
		RECT rc;

		GetLocaleInfo(
			MAKELCID(Lang, SORT_DEFAULT),
			LOCALE_SISO639LANGNAME,
			LangShort,
			250
			);
		CharUpperBuff(LangShort, 1);

		if (!mdc)
			mdc = GetDC(NULL);//ѕолучаем один раз при запуске. Ѕольше не отпускаем.
							  //Ёто по-идее должно решить проблемы при запущеных играх 
							  //с хитрым режимом экрана
							  //и создать новые, интересные проблемы!
							  //(например, при переключении режима вручную)
		bdc = CreateCompatibleDC(mdc);
		//ReleaseDC(NULL,mdc);
		lf.lfHeight = -MulDiv(7, GetDeviceCaps(bdc, LOGPIXELSY), 72);
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = 0; //maybe 400?
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = NONANTIALIASED_QUALITY;
		lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;

		StringCchCopyW(lf.lfFaceName, sizeof(lf.lfFaceName) / sizeof(lf.lfFaceName[0]), L"MS Sans Serif");  		//lstrcpy(lf.lfFaceName, L"MS Sans Serif");
		hf = CreateFontIndirect(&lf);
		SelectObject(bdc, hf);

		//LangBitmap=CreateCompatibleBitmap(bdc,16,16);
		LangBitmap = CreateBitmap(16, 16, 1, 32, NULL);
		SelectObject(bdc, LangBitmap);
		hBrush = CreateSolidBrush(ICONBGCOLOR);
		rc.top = 0;
		rc.left = 0;
		rc.bottom = 16;
		rc.right = 16;
		FillRect(bdc, &rc, hBrush);
		//TextOut(bdc, 0, 0, LangShort, wcslen(LangShort));
		SetTextColor(bdc, RGB(255, 255, 255));
		SetBkColor(bdc, ICONBGCOLOR);
		rc.top = 2;
		DrawText(bdc, LangShort, (int)wcslen(LangShort), &rc, 0);
		DeleteDC(bdc);
		DeleteObject(hBrush);
		DeleteObject(hf);

		ICONINFO ii = { 0 };
		ii.fIcon = TRUE;
		ii.hbmColor = LangBitmap;
		ii.hbmMask = CreateBitmap(16, 16, 1, 24, NULL);

		HICON hIcon = ::CreateIconIndirect(&ii);

		return hIcon;
	}

	void indicateLayout(HKL layout) {

		HICON icon = 0;
		tstring langid = layoutList::layoutLangId(layout);
		TLayoutIcons::const_iterator it = LayoutIcons.find(langid);
		if (it != LayoutIcons.end()) {
			// Icon is already loaded
			icon = it->second;
		} else {
			// Load icon

//			tstring path = TEXT("icons\\") + langid;
//			path += Application::GetApp()->isActive() ? TEXT(".ico") : TEXT("_grayscale.ico");

			icon = CreateLangIcon(layout);
			//icon = static_cast<HICON>(LoadImage(0, path.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_LOADTRANSPARENT));
			//assert(icon != NULL);
			//if (icon == NULL) {
				// If icon was not found, use our main icon
			//	icon = settings::Settings.mainIcon;
//			} else {
				// If icon is loaded, add it to the map
				// NOTE: We can pass our iterator here to increase performance
				LayoutIcons.insert(langid, icon);
			//}
		}
		// Change icon
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = TrayIconId;
		nid.uFlags = NIF_ICON;
		nid.hIcon = icon;
		verify(Shell_NotifyIcon(NIM_MODIFY, &nid));
	}

	void destroy() {
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = TrayIconId;
		verify(Shell_NotifyIcon(NIM_DELETE, &nid));
		Window = 0;

		verify(DestroyMenu(Menu));
		Menu = 0;

		TLayoutIcons::const_iterator it = LayoutIcons.begin();
		TLayoutIcons::const_iterator end = LayoutIcons.end();
		for (; it != end; ++it)
			DestroyIcon(it->second);
		LayoutIcons.clear();
	}

	HMENU getMenu() {
		return Menu;
	}
}