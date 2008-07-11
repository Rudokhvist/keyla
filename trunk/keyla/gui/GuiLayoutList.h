#pragma once

#include "GuiHotKey.h"
#include "../../win32xx/WinCore.h"

#include <vector>

class GuiLayoutList;

//
// Вспомотельныей класс элемента управления для выбора сочетания клавиш
// для любой раскладки в списке
//
class HotKeyEditDelegate : public GuiHotKey {
public:

	// Конструктор
	HotKeyEditDelegate();

	// Переопределение метода базового класса. Этому классу требуется
	// не просто HWND родительского окна, но ссылка на GuiLayoutList
	HWND Create(GuiLayoutList & layoutList);

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Ссылка на список раскладок, который владеет делегатом
	GuiLayoutList * m_layoutList;
};

//
// Класс элемента управления для вывода списка раскладок
// и редактирования для каждой из них сочетания клавиш
//
// Элемент управления привязывайте к существующему
// элементу управления ListView с помощью Attach.
// Удостоверьтесь, что элемент управления имеет стиль Report.
//
class GuiLayoutList : public CWnd{
public:

	// Конструктор
	GuiLayoutList();

	// Инициализировать содержимое элемента управления.
	// Метод update вызывается автоматически
	// settingsWindow - окно, в котором расположен элемент управления
	void initialize();

	// Обновить содержимое элемента управления
	void update();

	// Применить сделанные изменения (внести их в глобальные настройки)
	void apply();

	// Метод вызывается из объекта m_delegate, когда с делегата был снят фокус.
	// Введённую информацию заносим в таблицу, а делегат скрываем
	void delegateDeactivated();

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Локальная копия настроек сочетаний клавиш для раскладок
	std::vector<HotKey> m_layoutHotKeys;

	// Флажок, который устанавливается, есть идёт редакирования сочетания клавиш
	// (показан и активен делегат)
	bool m_delegateActive;

	// Вспомотельныей класс элемента управления для выбора сочетания клавиш
	// для любой раскладки в списке
	HotKeyEditDelegate m_delegate;
	
	// Ряд, в котором располагается делегат, или 0, если в данный момент делегат скрыт
	unsigned int m_delegateRow;
};