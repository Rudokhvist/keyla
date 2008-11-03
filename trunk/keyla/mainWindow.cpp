#include "common.h"
#include "application.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "settingsWindow.h"
#include "trayIcon.h"
#include "../win32xx/WinCore.h"

class MainWindow : public CWnd {
protected:

	virtual void PreRegisterClass(WNDCLASS & wc) {
		CWnd::PreRegisterClass(wc);

		// Use partucular window class to be able to find a running application instance easily
		wc.lpszClassName = TEXT("keyla main window");
	}

	virtual void PreCreate(CREATESTRUCT & cs) {
		// Window only handles messages. It does not interact with user, so hide it.
		cs.style &= ~WS_VISIBLE;

		// NOTE: CWnd overwrites our changes in cs.style if it is zero. So let it be non-zero.
		cs.style |= WS_BORDER;

		// NOTE: Due to CWnd's implementation we first need to change cs
		// and after that call the base class member
		CWnd::PreCreate(cs);
	}

	virtual void OnCreate() {
		CWnd::OnCreate();
		trayIcon::create(GetHwnd());
	}

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

		// Message from the tray icon
		if (message == mainWindow::TrayIconMessage && wparam == mainWindow::TrayIconId)
			// NOTE: Do not allow to touch the icon when the settings window is shown
			switch (lparam) {
				case WM_CONTEXTMENU:
				case WM_RBUTTONUP:
					if (settingsWindow::isShown()) 
						MessageBeep(-1);
					else
						trayIcon::showMenu();
					return 0;
				case NIN_KEYSELECT:
				case NIN_SELECT:
				case WM_LBUTTONUP:
					if (settingsWindow::isShown()) 
						MessageBeep(-1);
					else
						settingsWindow::show();
					return 0;
			}
		
		if (message == WM_DESTROY) {
		
			// Destroy the tray icon. We do it here as we should pass main window's HWND.
			// Otherwise it would be better to do it in the MAIN module.
			trayIcon::destroy();

			// Quit the application
			PostQuitMessage(0);
			return 0;
		}

		return CWnd::WndProc(window, message, wparam, lparam);
	}

	virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) {
		unsigned int code = HIWORD(wparam);
		unsigned int id = LOWORD(wparam);
		HWND hwnd = reinterpret_cast<HWND>(lparam);

		// Message from the contenxt menu of the tray icon
		if (hwnd == 0 && code == 0)
			switch (id) {
				case ID_TRAYICONMENU_TOGGLE: {
					Application::GetApp()->toggle();

					MENUITEMINFO mii = {sizeof(mii), MIIM_TYPE, MFT_STRING};
					mii.dwTypeData = Application::GetApp()->isActive() ? _T("Отключить") : _T("Включить");
					SetMenuItemInfo(trayIcon::getMenu(), id, FALSE, &mii);

					return 0;
				}
				case ID_TRAYICONMENU_SETTINGS:
					settingsWindow::show();
					return 0;
				case ID_TRAYICONMENU_EXIT:
					DestroyWindow();
					return 0;
			}
		return CWnd::OnCommand(wparam, lparam);
	}

} MyMainWindow;

namespace mainWindow {

	void create() {
		MyMainWindow.Create();
	}

}