#include "common.h"
#include "core.h"
#include "layoutList.h"
#include "settings.h"
#include "trayIcon.h"

#include <algorithm>
using namespace std;

// Запоминаем раскладку, на которую провели переключение,
// чтобы затем в функции layoutChanged это изменение разрешить
static HKL ExpectedLayout;

// Установить в заданном окне переданную раскладку.
// Функция обновляет глобальную переменную ExpectedLayout.
//
// window - окно
// layout - раскладка
//
void setLayout(HWND window, HKL layout) {
	ExpectedLayout = layout;
	PostMessage(window, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(layout));
}

namespace core {

	void nextLayout() {
		HWND hwnd = GetForegroundWindow();
		HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(hwnd, 0));
		
		vector<HKL>::iterator begin = layoutList::LayoutList.begin();
		vector<HKL>::iterator end = layoutList::LayoutList.end();
		vector<HKL>::iterator it = find(begin, end, layout);
		if (it == end) {
			assert(false);
			return;
		}
		++it;
		if (it == end) {
			it = begin;
		}
		::setLayout(hwnd, *it);
	}

	void setLayout(unsigned int index) {
		::setLayout( GetForegroundWindow(), layoutList::LayoutList[index] );
	}

	void activeWindowChanged(HWND activeWindow, HKL layout) {
		if (ExpectedLayout != 0 && settings::Settings.globalLayout) {
			// Переключаем раскладку в этом окне на "глобальную".
			// Установку нужной иконки в трее и прочую индикацию проведем  в layoutChanged,
			// куда мы обязательно попадём
			::setLayout(activeWindow, ExpectedLayout);
			return;
		}

		// Сохраняем раскладку и устанавливаем индикацию
		ExpectedLayout = layout;
		trayIcon::indicateLayout(layout);
	}

	bool layoutChanged(HKL layout) {
		bool ret;
		if (layout == ExpectedLayout) {
			// Разрешить переключение
			ret = false;
		}
		else {			
			// Иначе действуем в соответствии с настройками
			ret = settings::Settings.skipSystemHotKey;
		}
		if (ret == false) {
			// Если разрешили, устанавливаем индикацию этой раскладки
			trayIcon::indicateLayout(layout);
		}
		return ret;
	}

	bool keyPressed(unsigned int vk, unsigned int modifiers) {
		
		HotKey h(vk, modifiers);
		if (settings::Settings.mainHotKey == h) {
			nextLayout();
			return true;
		}

		vector<HotKey>::const_iterator begin = settings::Settings.layoutHotKeys.begin();
		vector<HotKey>::const_iterator end = settings::Settings.layoutHotKeys.end();
		vector<HotKey>::const_iterator it = find(begin, end, h);
		if (it != end) {
			setLayout(static_cast<unsigned int>(distance(begin, it)));
			return true;
		}

		return false;
	}

}