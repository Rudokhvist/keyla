#include "../common.h"
#include "GuiSettingsWindow.h"
#include "../settings.h"
#include "../res/resource.h"

#include <algorithm>
#include <functional>
using namespace std;

GuiSettingsWindow::GuiSettingsWindow() : CPropertySheet(LoadStringLang(IDS_KEYLA_SETTINGS).c_str()) {

	m_PSH.dwFlags |= PSH_USEHICON;
	m_PSH.hIcon = settings::Settings.mainIcon;
	
	AddPage(new CommonPropsPage(*this));
	AddPage(new LayoutPropsPage(*this));
}

void GuiSettingsWindow::apply() {
	vector<CPropertyPage *>::iterator it = m_vPages.begin();
	vector<CPropertyPage *>::iterator end = m_vPages.end();
	for (; it < end; ++it) {
		static_cast<Page *>(*it)->apply();
	}
	settings::save();
}


GuiSettingsWindow::Page::Page(GuiSettingsWindow &parent, int resourceId, LPCTSTR title) : CPropertyPage(resourceId, title), m_parent(&parent) {
}

/* virtual */ void GuiSettingsWindow::Page::OnSetActive() {
	CPropertyPage::OnSetActive();
	
	// Let the Apply button to be always active
	SetModified(true);
}

/* virtual */ void GuiSettingsWindow::Page::OnApply() {
	m_parent->apply();
	CPropertyPage::OnApply();
}

/* virtual */ void GuiSettingsWindow::Page::OnOK() {
	m_parent->apply();
	CPropertyPage::OnOK();
}

	
GuiSettingsWindow::CommonPropsPage::CommonPropsPage(GuiSettingsWindow &parent) :
	Page(parent, IDD_COMMONPROPS, LoadStringLang(IDS_COMMON).c_str()) {
}

/* virtual */ BOOL GuiSettingsWindow::CommonPropsPage::OnInitDialog() {
	Page::OnInitDialog();

	verify(m_mainKeyEdit.Attach(GetDlgItem(GetHwnd(), IDC_COMMONPROPS_EDIT_KEY)));
	m_mainKeyEdit.setHotKey(settings::Settings.mainHotKey);

	if (settings::Settings.globalLayout) {
		CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_GLOBALLAYOUT, BST_CHECKED);
	}
	if (settings::Settings.skipSystemHotKey) {
		CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_EATWINDOWSKEY, BST_CHECKED);
	}
	if (settings::Settings.noleftright) {
		CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_NOLEFTRIGHT, BST_CHECKED);
	}

	return TRUE;
}

/* virtual */ void GuiSettingsWindow::CommonPropsPage::apply() {
	settings::Settings.mainHotKey = m_mainKeyEdit.hotKey();
	settings::Settings.globalLayout = (IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_GLOBALLAYOUT) != 0);
	settings::Settings.noleftright = (IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_NOLEFTRIGHT) != 0);
	settings::Settings.skipSystemHotKey = (IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_EATWINDOWSKEY) != 0);
}


GuiSettingsWindow::LayoutPropsPage::LayoutPropsPage(GuiSettingsWindow &parent) :
	Page(parent, IDD_LAYOUTPROPS, LoadStringLang(IDS_LAYOUTS).c_str()) {
}

/* virtual */ BOOL GuiSettingsWindow::LayoutPropsPage::OnInitDialog() {
	Page::OnInitDialog();

	verify(m_layoutList.Attach(GetDlgItem(GetHwnd(), IDC_LAYOUTPROPS_LIST_LAYOUTS)));
	m_layoutList.initialize();

	return TRUE;
}

/* virtual */ void GuiSettingsWindow::LayoutPropsPage::apply() {
	m_layoutList.apply();
}
