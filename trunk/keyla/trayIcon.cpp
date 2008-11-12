#include "common.h"
#include "application.h"
#include "layoutList.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "settings.h"
#include "trayIcon.h"

#include <algorithm>
#include <map>
using namespace std;

// Tray icon menu
static HMENU Menu = 0;

// Main window's HWND. Passed through create()
static HWND Window = 0;

// Tray icon's tooltip
static const tstring Tooltip = LoadStringLang(IDS_KEYLA_IS_A_KEYBOARD_LAYOUT_SWITCHER);

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
};
static TLayoutIcons LayoutIcons; 

namespace trayIcon {

	void create(HWND mainWindow) {
		Window = mainWindow;

		// Load the context menu from the resources
		Menu = LoadMenuIndirect(LoadResourceLang(RT_MENU, MAKEINTRESOURCE(IDM_TRAYICONMENU)));
		assert(Menu != 0);

		// Add the tray icon to the tray.
		// Do not set an image here in order to eliminete flickering. We will always do it in indicateLayout()
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = mainWindow::TrayIconId;
		nid.uFlags = NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = mainWindow::TrayIconMessage;
		_tcsncpy(nid.szTip, Tooltip.c_str(), sizeof(nid.szTip) / sizeof(TCHAR));
		verify(Shell_NotifyIcon(NIM_ADD, &nid));
	}

	void showMenu() {
		showContextMenu(Menu, Window);
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

			tstring path = TEXT("icons\\") + langid;
			path += Application::GetApp()->isActive() ? TEXT(".ico") : TEXT("_grayscale.ico");

			icon = static_cast<HICON>(LoadImage(0, path.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_LOADTRANSPARENT));
			if (icon == 0) {
				// If icon was not found, use our main icon
				assert(false);
				icon = settings::Settings.mainIcon;
			} else {
				// If icon is loaded, add it to the map
				// NOTE: We can pass our iterator here to increase performance
				LayoutIcons.insert(langid, icon);
			}
		}
		// Change icon
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = mainWindow::TrayIconId;
		nid.uFlags = NIF_ICON;
		nid.hIcon = icon;
		verify(Shell_NotifyIcon(NIM_MODIFY, &nid));
	}

	void destroy() {
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = mainWindow::TrayIconId;
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