#pragma once

#include "GuiLayoutList.h"
#include "GuiHotKey.h"
#include "../../win32xx/PropertySheet.h"

class GuiSettingsWindow : public CPropertySheet {
public:

	// Default contstructor
	GuiSettingsWindow();

	// Apply changes. This method gets called from an instance of FirstPage
	void apply();

private:

	// Class of a Page
	class Page : public CPropertyPage {
	public:

		// Constructor
		Page(GuiSettingsWindow & parent, int resourceId, LPCTSTR title);

		virtual void OnSetActive();
		virtual void OnApply();
		virtual void OnOK();

		// Apply changes. This method gets called from an instance of GuiSettingsWindow
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
