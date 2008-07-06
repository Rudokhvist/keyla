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

		// Используем определённый класс окна, чтобы по его названию
		// в случае чего искать запущенное приложение
		wc.lpszClassName = TEXT("keyla main window");
	}

	virtual void PreCreate(CREATESTRUCT & cs) {
		CWnd::PreCreate(cs);

		// Окно занимается только обработкой сообщений. С пользователем
		// напрямую не взаимодействует, поэтому должно быть скрыто
		cs.style &= ~WS_VISIBLE;
	}

	virtual void OnCreate() {
		CWnd::OnCreate();
		trayIcon::create(GetHwnd());
	}

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

		// Сообщение от иконки в трее
		if (message == mainWindow::TrayIconMessage && wparam == mainWindow::TrayIconId)
			switch (lparam) {
				case WM_CONTEXTMENU:
				case WM_RBUTTONUP:
					trayIcon::showMenu();
					return 0;
				case NIN_KEYSELECT:
				case NIN_SELECT:
				case WM_LBUTTONUP:
					settingsWindow::show();
					return 0;
			}
		
		if (message == WM_DESTROY) {
		
			// Деинициализация иконки в трее. Её нельзя произвести в модуле MAIN,
			// т.к. иконка привязана к HWND окна, а в модуле MAIN его получить не получится
			trayIcon::destroy();

			// Завершение приложения
			PostQuitMessage(0);
			return 0;
		}

		return CWnd::WndProc(window, message, wparam, lparam);
	}

	virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam) {
		unsigned int code = HIWORD(wparam);
		unsigned int id = LOWORD(wparam);
		HWND hwnd = reinterpret_cast<HWND>(lparam);

		// Сообщения от меню (иконки в трее)
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