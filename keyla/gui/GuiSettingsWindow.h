#pragma once

#include "GuiLayoutList.h"
#include "GuiHotKey.h"
#include "../../win32xx/PropertySheet.h"

class GuiSettingsWindow : public CPropertySheet {
public:

	// Конструтор
	GuiSettingsWindow();

	// Применить сделанные изменения. Метод вызывается из объекта класса FirstPage
	void apply();

private:

	// Класс, от которого должен наследоваться класс первой страницы
	class Page : public CPropertyPage {
	public:

		// Конструктор
		Page(GuiSettingsWindow & parent, int resourceId, LPCTSTR title);

		virtual void OnSetActive();
		virtual void OnApply();
		virtual void OnOK();

		// Сохранить сделанные изменения. Метод вызывается из объекта класса GuiSettingsWindow
		virtual void apply() = 0;

	private:

		GuiSettingsWindow * m_parent;
	};

	class CommonPropsPage : public Page {
	public:

		CommonPropsPage(GuiSettingsWindow & parent);

		virtual BOOL OnInitDialog();

		virtual void apply();

	private:		

		GuiHotKey m_mainKeyEdit;
	};

	class LayoutPropsPage : public Page {
	public:

		LayoutPropsPage(GuiSettingsWindow & parent);

		virtual BOOL OnInitDialog();

		virtual void apply();

	private:

		GuiLayoutList m_layoutList;
	};
};
