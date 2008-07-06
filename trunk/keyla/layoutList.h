//
// Модуль LAYOUT_LIST организует получение списка раскладок,
// включённых пользователем в системе
// 

#pragma once

#include <vector>

namespace layoutList {

	// Объект для хранения списка раскладок
	extern std::vector<HKL> LayoutList;

	// Загрузить список раскладок
	void load();

	// Вспомогательная функция, возвращающая language id раскладки в виде строки
	tstring layoutLangId(HKL layout);

	// Вспомогательная функция, возвращающая название языка, соответствующего раскладке
	tstring layoutLanguage(HKL layout);
}