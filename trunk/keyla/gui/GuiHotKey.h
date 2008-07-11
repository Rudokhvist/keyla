#pragma once

#include "Menu.h"
#include "../util/HotKey.h"
#include "../../win32xx/WinCore.h"

//
// Класс элемента управления для ввода сочетания клавиш
//
// Элемент управления можно создать с помощью метода Create
// или привязать к существующему полю ввода с помощью Attach.
// При использовании Create элемент управления создаётся невидимым.
// При использовании Attach удостоверьтесь, что поле ввода отключено (disabled)
//
class GuiHotKey : public CWnd{
public:

	// Конструктор
	GuiHotKey();

	// Установить сочетание клавиш
	void setHotKey(const HotKey & hotKey);

	// Установить сочетание клавиш.
	// Описание параметров см. в комментариях к методу HotKey::set
	void setHotKey(unsigned int vk, unsigned int modifiers);

	// Очистить сочетание клавиш.
	// Описание параметров см. в комментариях к методу HotKey::set
	void clearHotKey();

	// Получить ссылку на объект, в котором хранится установленное сочетание клавиш
	const HotKey & hotKey() const;

protected:

	virtual void PreCreate(CREATESTRUCT & cs);
	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Инициализировать элемент управления
	void initialize();

	// Значение, которое используется для определения необходимости
	// вызова метода initialize в функции окна
	HWND m_lastInitializedHwnd;

	// Установленное сочетание клавиш
	HotKey m_hotKey;


	// Указатель на экземпляр класса, который соответствует элементу управления,
	// на котором в настоящий момент находится фокус. Или нулевой указатель
	static GuiHotKey * ActiveInstance;


	// Хук на клавиутуру. Выбран тип хука WH_KEYBOARD_LL, поскольку только
	// с его помощью можно отловить такие "системные" сочетания клавиш, как Win-D
	static LRESULT CALLBACK keyboardHook(int code, WPARAM wparam, LPARAM lparam);

	// Хук на клавиатуру
	static HHOOK KeyboardHook;

	// Модификаторы, которые набрали за время действия хука
	// (набор флагов HotKey::Modifiers)
	static unsigned int Modifiers;

	// Контекстное меню
	static Menu ContextMenu;
};