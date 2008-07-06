#pragma once

#include "HotKeyEdit.h"
#include "../../win32pp/WinCore.h"

class LayoutList;
class SettingsDialog;

//
// Вспомотельныей класс элемента управления для выбора сочетания клавиш
// для любой раскладки в списке
//
class HotKeyEditDelegate : public HotKeyEdit {
public:

	// Конструктор
	HotKeyEditDelegate();

	// Переопределение метода базового класса. Этому классу требуется
	// не просто HWND родительского окна, но ссылка на LayoutList
	HWND Create(LayoutList & layoutList);

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Ссылка на список раскладок, который владеет делегатом
	LayoutList * m_layoutList;
};

//
// Класс элемента управления для вывода списка раскладок
// и редактирования для каждой из них сочетания клавиш
//
// Элемент управления привязывайте к существующему
// элементу управления ListView с помощью Attach.
// Удостоверьтесь, что элемент управления имеет стиль Report.
//
class LayoutList : public CWnd{
public:

	// Конструктор
	LayoutList();

	// Инициализировать содержимое элемента управления.
	// Метод update вызывается автоматически
	// settingsWindow - окно, в котором расположен элемент управления
	void initialize(SettingsDialog & settingsDialog);

	// Обновить содержимое элемента управления
	void update();

	// Метод вызывается из объекта m_delegate, когда с делегата был снят фокус.
	// Введённую информацию заносим в таблицу, а делегат скрываем
	void delegateDeactivated();

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Диалоговое окно, которому принадлежим
	SettingsDialog * m_settingsDialog;

	// Флажок, который устанавливается, есть идёт редакирования сочетания клавиш
	// (показан и активен делегат)
	bool m_delegateActive;

	// Вспомотельныей класс элемента управления для выбора сочетания клавиш
	// для любой раскладки в списке
	HotKeyEditDelegate m_delegate;
	
	// Ряд, в котором располагается делегат, или 0, если в данный момент делегат скрыт
	unsigned int m_delegateRow;
};