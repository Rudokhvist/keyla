//
// Модуль SETTINGS заведует чтением настроек программы из конфигурационных файлов
// и записью настроек программы в них. Для этого модуль предоставляет функции load и save
//

#pragma once

#include "gui/Icon.h"
#include "util/HotKey.h"

#include <vector>

namespace settings {

	//
	// Единственный объект, хранящий настройки программы.
	// Инициализируется в методе load
	//
	extern struct SettingsStruct {
		SettingsStruct();

		// Сочетание клавиш для переключения на следующую раскладку
		HotKey mainHotKey;
		// Запретить использовать для переключения сочетание клавиш, установленное в системе
		bool skipSystemHotKey;
		// Использовать ли одну глобальную раскладку, а не свою для каждого окна
		bool globalLayout;
		// Сочетания клавиш для переключения на конкретную раскладку
		std::vector<HotKey> layoutHotKeys;
		// Главная иконка приложения
		Icon mainIcon;
	} Settings;

	void load();

	void save();
}