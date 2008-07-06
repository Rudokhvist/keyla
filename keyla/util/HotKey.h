//
// Вспомогательный модуль HOTKEY предоставляет класс HotKey
//

#pragma once

// Класс для хранения сочетания клавиш
class HotKey {
public:

	// Модификаторы клавиш
	enum Modifiers {
		// Левый Ctrl
		LControl  = 1,
		// Левый Alt
		LAlt      = 2,
		// Левый Shift
		LShift    = 4,
		// Правый Ctrl
		RControl  = 8,
		// Правый Alt
		RAlt      = 16,
		// Правый Shift
		RShift    = 32,
		// Флажок "расширенной клавиши"
		Extended  = 128,
	};

	// Получить название клавиши для отображения пользователю
	// vk - виртуальный код клавиши
	// extended - флаг "расширенной клавиши"
	static tstring keyName(unsigned int vk, bool extended = false);


	// Конструктор
	HotKey();

	// Конструктор
	// vk - виртуальный код клавиши
	// modifiers - набор флагов из перечисления Modifiers
	HotKey(unsigned int vk, unsigned int modifiers);


	bool operator==(const HotKey & other) const;


	// Метод-"сеттер"
	// vk - виртуальный код клавиши
	// modifiers - набор флагов из перечисления Modifiers
	void set(unsigned int vk, unsigned int modifiers);

	// Очистить сочетание клавиш
	void clear();


	// Получить виртуальный код клавиши
	unsigned int vk() const;

	// Получить модификаторы - набор флагов из перечисления Modifiers
	unsigned int modifiers() const;

	// Получить строку, описывающую сочетание клавиш, для показа пользователю
	const tstring & text() const;
	

	// Загрузить данные из реестра
	// hkey - ключ или раздел реестра (HKEY_CURRENT_USER)
	// key - подключ реестра
	void loadFromRegistry(HKEY hkey, const tstring & key);

	// Загрузить данные в реестр
	// hkey - ключ или раздел реестра (HKEY_CURRENT_USER)
	// key - подключ реестра
	void saveToRegistry(HKEY hkey, const tstring & key);

private:

	// Заполнить m_text согласно m_vk и m_modifiers
	void makeText();
	
	// виртуальный код клавиши
	unsigned int m_vk;

	// набор флагов из перечисления Modifiers
	unsigned int m_modifiers;

	// Строка, описывающая сочетание клавиш, для показа пользователю
	// NOTE: Обновлять при изменении m_modifiers и/или m_vk
	tstring m_text;

	// Разделитель в текстовом представлении сочетания клавиш
	static const tstring Separator;
};
