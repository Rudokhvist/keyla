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

// Меню иконки в трее
static HMENU Menu = 0;

// HWND главного окна. Передаётся в методе create
static HWND Window = 0;

// Всплывающая подсказка у иконки
static const LPCTSTR Tooltip = TEXT("keyla - переключатель раскладок клавиатуры");

// Словарь с иконками для каждой раскладки
// ключ - строковое представление language id ракладки
// значение - описатель иконки
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

		// Загружаем из ресурсов меню иконки в трее
		Menu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(IDM_TRAYICONMENU));
		assert(Menu != 0);

		// Добавляем иконку в трей.
		// Чтобы избежать мелькания, не устанавливаем сразу картинку,
		// сделаем это в indicateLayout по-любому
		NOTIFYICONDATA nid = {sizeof nid};
		nid.hWnd = Window;
		nid.uID = mainWindow::TrayIconId;
		nid.uFlags = NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = mainWindow::TrayIconMessage;
		_tcsncpy(nid.szTip, Tooltip, sizeof(nid.szTip) / sizeof(TCHAR));
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
			// Иконка уже была загружена
			icon = it->second;
		} else {
			// Загружаем иконку

			tstring path = TEXT("icons\\") + langid;
			path += Application::GetApp()->isActive() ? TEXT(".ico") : TEXT("_grayscale.ico");

			icon = static_cast<HICON>(LoadImage(0, path.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_LOADTRANSPARENT));
			if (icon == 0) {
				// Иконка не найдена. Используем нашу стандартную иконку
				assert(false);
				icon = settings::Settings.mainIcon;
			} else {
				// Иконка загружена. Добавляем её в словарь
				// NOTE: Интересно, передача в insert нашего итератора ускорит вставку???
				LayoutIcons.insert(langid, icon);
			}
		}
		// Собственно изменяем иконку
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