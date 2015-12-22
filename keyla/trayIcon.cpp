#if _WIN32_WINNT<0x501
#define _WIN32_WINNT 0x501
#endif

#include <windows.h> 
#include "common.h"
#include "application.h"
#include "layoutList.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "settings.h"
#include "settingsWindow.h"
#include "trayIcon.h"
#include "strsafe.h"
#include <Winbase.h>
#include <algorithm>
#include <map>


#ifdef _DEBUG
#define ICONBGCOLOR RGB(0,128,128)
#else
#define ICONBGCOLOR RGB(0,0,128)
#endif

#define ICONGRAY RGB (96,96,96)

typedef struct {
	UINT cbSize;
	DWORD dwMask;
	int idCommand;
	int iImage;
	BYTE fsState;
	BYTE fsStyle;
	WORD cx;
	unsigned __int64 lParam;
	unsigned __int64 pszText;
	int cchText;
} TBBUTTONINFO64, *LPTBBUTTONINFO64;

typedef struct _TNPRIVICON
{
	HWND hWnd;
	UINT uID;
	UINT uCallbackMessage;
	DWORD dwState;
	UINT uVersion;
	HICON hIcon;
} TNPRIVICON, *PTNPRIVICON;

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

BOOL DoesWin32MethodExist(PCWSTR pszModuleName, PCSTR pszMethodName)
{
	HMODULE hModule = GetModuleHandle(pszModuleName);
	if (hModule == NULL)
	{
		return FALSE;
	}
	return (GetProcAddress(hModule, pszMethodName) != NULL);
}

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

	HICON CreateLangIcon(HKL Lang, COLORREF bgcolor)
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
			mdc = GetDC(NULL);//Get DC once on start, and don't release it
							  //This can fix the problem with some games
							  //changing screen mode
							  //and create new and interesting bugs!
							  //(maybe when changing screen mode manually?)
		bdc = CreateCompatibleDC(mdc);
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

		StringCchCopyW(lf.lfFaceName, sizeof(lf.lfFaceName) / sizeof(lf.lfFaceName[0]), L"MS Sans Serif");
		hf = CreateFontIndirect(&lf);
		SelectObject(bdc, hf);

		LangBitmap = CreateBitmap(16, 16, 1, 32, NULL);
		SelectObject(bdc, LangBitmap);
		hBrush = CreateSolidBrush(bgcolor);
		rc.top = 0;
		rc.left = 0;
		rc.bottom = 16;
		rc.right = 16;
		FillRect(bdc, &rc, hBrush);
		SetTextColor(bdc, RGB(255, 255, 255));
		SetBkColor(bdc, bgcolor);
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

		static BOOL fFlipFlop = TRUE;
		static int prev_pos = 0;
		HICON icon = 0;
		HWND TrayNotifyHandle;
		HWND OSDependence;
		HWND FSysTrayHandle;
		int count;
		int pos;
		HANDLE hTrayProc;
		DWORD dwTrayProcessID;
		void* lpData;
		void* icoData;
		TBBUTTONINFO64 mtbbi;
		SIZE_T dwBytesRead = -1;
		LPTBBUTTONINFO ptb;
		TNPRIVICON pico;

		BOOL is64bit = ((DoesWin32MethodExist(L"kernel32.dll", "IsWow64Process") && IsWow64Process(GetCurrentProcess(), &is64bit)) && is64bit);


		tstring langid = layoutList::layoutLangId(layout);
		TLayoutIcons::const_iterator it = LayoutIcons.find(langid);
		if (it != LayoutIcons.end()) {
			// Icon is already loaded
			icon = it->second;
		} else {

			icon = CreateLangIcon(layout, Application::GetApp()->isActive()? ICONBGCOLOR:ICONGRAY);
			LayoutIcons.insert(langid, icon);

		}

		// Change icon
		NOTIFYICONDATA nid = { sizeof nid };
		nid.hWnd = Window;
		nid.uID = TrayIconId;
		nid.uFlags = NIF_ICON;
		nid.hIcon = icon;

		if (fFlipFlop) {//if icon already was set - check it's position
			TrayNotifyHandle = FindWindow(L"Shell_TrayWnd", NULL);
			if (TrayNotifyHandle) {
				TrayNotifyHandle = FindWindowEx(TrayNotifyHandle, 0, L"TrayNotifyWnd", NULL);
				if (TrayNotifyHandle) {
					OSDependence = FindWindowEx(TrayNotifyHandle, 0, L"SysPager", NULL);
					if (!OSDependence) {
						// W2K don't have SysPager
						OSDependence = TrayNotifyHandle;
					}
					FSysTrayHandle = FindWindowEx(OSDependence, 0, L"ToolbarWindow32", NULL);
				}
			}
			count = (int)SendMessage(FSysTrayHandle, TB_BUTTONCOUNT, 0, 0);
			pos = -1;
			//Dirty hack to find our icon position
			GetWindowThreadProcessId(FSysTrayHandle, &dwTrayProcessID);
			hTrayProc = OpenProcess(PROCESS_ALL_ACCESS, 0, dwTrayProcessID);
			if (hTrayProc) {
				if (lpData = VirtualAllocEx(hTrayProc, 0, sizeof(TBBUTTONINFO64), MEM_COMMIT, PAGE_READWRITE)) {
					for (int i = 0; i<count; i++) {
						memset(&mtbbi, 0, sizeof(mtbbi));
						if (is64bit)
							mtbbi.cbSize = sizeof(TBBUTTONINFO64);
						else
							mtbbi.cbSize = sizeof(TBBUTTONINFO);
						mtbbi.dwMask = TBIF_BYINDEX | TBIF_LPARAM;
						if (WriteProcessMemory(hTrayProc, lpData, &mtbbi, sizeof(TBBUTTONINFO64), &dwBytesRead)) {
							if (SendMessage(FSysTrayHandle, TB_GETBUTTONINFO, i, (LPARAM)lpData) >= 0) {
								// read data from another process
								if (ReadProcessMemory(hTrayProc, lpData, &mtbbi, sizeof(TBBUTTONINFO64), &dwBytesRead)) {
									icoData = NULL;
									if (is64bit) {
										if (!(int)*(((int*)&mtbbi.lParam) + 1))
											icoData = (void*)mtbbi.lParam;
									}
									else {
										ptb = (LPTBBUTTONINFO)&mtbbi;
										icoData = (void*)((LPTBBUTTONINFO)&mtbbi)->lParam;
									}
									if (icoData && ReadProcessMemory(hTrayProc, icoData, &pico, sizeof(TNPRIVICON), &dwBytesRead)) {
										if ((pico.hWnd != 0) && (pico.hWnd == Window)) {
											//it's our window
											pos = i;
											break;
										}
									} //else throw std::exception("ReadMemory2"); //ReadMemory
								} //else throw std::exception("ReadMemory");//ReadMemory
							} //else throw std::exception("SendMessage");//SendMessage
						} //else throw std::exception("WriteProcessMemory");//WriteProcessMemory
					} //for
					VirtualFreeEx(hTrayProc, lpData, 0, MEM_RELEASE);
					CloseHandle(hTrayProc);
				} //else throw std::exception("lpData");//if (lpData)
			} //else throw std::exception("hTrayProc");//if (hTrayProc)
			if ((pos<0) || (pos>prev_pos)) {
				//We are not first - try to redraw
				fFlipFlop = FALSE;
				Shell_NotifyIcon(NIM_DELETE, &nid);
			}
			if (pos<0) pos = 0;
			prev_pos = pos;
		} //if (fFlipFlop)	

		if (fFlipFlop) {
			verify(Shell_NotifyIcon(NIM_MODIFY, &nid));
		} else {
		 	nid.uFlags = NIF_MESSAGE | NIF_TIP;
		    nid.uCallbackMessage = TrayIconMessage;
			_tcsncpy(nid.szTip, Tooltip.c_str(), sizeof(nid.szTip) / sizeof(TCHAR));
			verify(Shell_NotifyIcon(NIM_ADD, &nid));
			fFlipFlop = TRUE;
		}

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
