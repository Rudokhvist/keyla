#include "common.h"
#include "core.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "settingsWindow.h"
#include "trayIcon.h"
#include "../win32pp/WinCore.h"

class MainWindowClass : public CWnd {
protected:

	virtual void PreRegisterClass(WNDCLASS & wc) {
		CWnd::PreRegisterClass(wc);

		// »спользуем определЄнный класс окна, чтобы по его названию
		// в случае чего искать запущенное приложение
		wc.lpszClassName = TEXT("keyla main window");
	}

	virtual void PreCreate(CREATESTRUCT & cs) {
		CWnd::PreCreate(cs);

		// ќкно занимаетс€ только обработкой сообщений. — пользователем
		// напр€мую не взаимодействует, поэтому должно быть скрыто
		cs.style &= ~WS_VISIBLE;
	}

	virtual void OnCreate() {
		CWnd::OnCreate();
		trayIcon::create(GetHwnd());
	}

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

		// —ообщение от иконки в трее
		if (message == mainWindow::TrayIconMessage && wparam == mainWindow::TrayIconId)
			// NOTE: ¬о избежание недоразумений, не позвол€ем использовать иконку, когда активно окно настроек
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
		
			// ƒеинициализаци€ иконки в трее. ≈Є нельз€ произвести в модуле MAIN,
			// т.к. иконка прив€зана к HWND окна, а в модуле MAIN его получить не получитс€
			trayIcon::destroy();

			// «авершение приложени€
			PostQuitMessage(0);
			return 0;
		}

		return CWnd::WndProc(window, message, wparam, lparam);
	}

	virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) {
		unsigned int code = HIWORD(wparam);
		unsigned int id = LOWORD(wparam);
		HWND hwnd = reinterpret_cast<HWND>(lparam);

		// —ообщени€ от меню (иконки в трее)
		if (hwnd == 0 && code == 0)
			switch (id) {
				case ID_TRAYICONMENU_SETTINGS:
					settingsWindow::show();
					return 0;
				case ID_TRAYICONMENU_EXIT:
					DestroyWindow();
					return 0;
			}
		return CWnd::OnCommand(wparam, lparam);
	}

} MainWindow;

namespace mainWindow {

	void create() {
		MainWindow.Create();
	}

}