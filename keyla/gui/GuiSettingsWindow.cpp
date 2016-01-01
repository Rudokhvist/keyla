#include "../common.h"
#include "GuiSettingsWindow.h"
#include "../settings.h"
#include "../res/resource.h"

#include <algorithm>
#include <functional>
using namespace std;


GuiSettingsWindow::GuiSettingsWindow() : CPropertySheet((std::wstring(_T("keyla ver. "))+std::wstring(_T(VERSIONSTR))).c_str()) {

	SetIcon(IDI_MAINICON);
	AddPage(new CommonPropsPage(*this));
	AddPage(new LayoutPropsPage(*this));
}

void GuiSettingsWindow::apply() {
/*	vector<CPropertyPage *>::iterator it = m_vPages.begin();
	vector<CPropertyPage *>::iterator end = m_vPages.end();
	for (; it < end; ++it) {
		static_cast<Page *>(*it)->apply();
	}
	*/
	//This is stupid, but what can I do?
	int prevpage = GetPageIndex(GetActivePage());
	for (int i = 0; i < GetPageCount(); i++) {
		SetActivePage(i);
		static_cast<Page *>(GetActivePage())->apply();
	}
	SetActivePage(prevpage);
	settings::save();
}


GuiSettingsWindow::Page::Page(GuiSettingsWindow &parent, int resourceId, LPCTSTR title) : CPropertyPage(resourceId, title), m_parent(&parent) {
}

/* virtual */ int GuiSettingsWindow::Page::OnSetActive() {
	CPropertyPage::OnSetActive();
	
	// Let the Apply button to be always active
	SetModified(true);
	return 0;
}

/* virtual */ int GuiSettingsWindow::Page::OnApply() {
	m_parent->apply();
	CPropertyPage::OnApply();
	return 0;
}

/* virtual */ int GuiSettingsWindow::Page::OnOK() {
	m_parent->apply();
	CPropertyPage::OnOK();
	return 0;
}

	
GuiSettingsWindow::CommonPropsPage::CommonPropsPage(GuiSettingsWindow &parent) :
	Page(parent, IDD_COMMONPROPS, LoadStringLang(IDS_COMMON).c_str()) {
}

/* virtual */ BOOL GuiSettingsWindow::CommonPropsPage::OnInitDialog() {
	Page::OnInitDialog();

	verify(m_mainKeyEdit.Attach(::GetDlgItem(GetHwnd(), IDC_COMMONPROPS_EDIT_KEY)));
	m_mainKeyEdit.setHotKey(settings::Settings.mainHotKey);

	if (settings::Settings.globalLayout) {
		::CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_GLOBALLAYOUT, BST_CHECKED);
	}
	if (settings::Settings.skipSystemHotKey) {
		::CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_EATWINDOWSKEY, BST_CHECKED);
	}
	if (settings::Settings.noleftright) {
		::CheckDlgButton(GetHwnd(), IDC_COMMONPROPS_CHECK_NOLEFTRIGHT, BST_CHECKED);
	}

	return TRUE;
}

/* virtual */ void GuiSettingsWindow::CommonPropsPage::apply() {
	settings::Settings.mainHotKey = m_mainKeyEdit.hotKey();
	settings::Settings.globalLayout = (::IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_GLOBALLAYOUT) != 0);
	settings::Settings.noleftright = (::IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_NOLEFTRIGHT) != 0);
	settings::Settings.skipSystemHotKey = (::IsDlgButtonChecked(GetHwnd(), IDC_COMMONPROPS_CHECK_EATWINDOWSKEY) != 0);
}


GuiSettingsWindow::LayoutPropsPage::LayoutPropsPage(GuiSettingsWindow &parent) :
	Page(parent, IDD_LAYOUTPROPS, LoadStringLang(IDS_LAYOUTS).c_str()) {
}

/* virtual */ BOOL GuiSettingsWindow::LayoutPropsPage::OnInitDialog() {
	Page::OnInitDialog();

	verify(m_layoutList.Attach(::GetDlgItem(GetHwnd(), IDC_LAYOUTPROPS_LIST_LAYOUTS)));
	m_layoutList.initialize();

	return TRUE;
}

/* virtual */ void GuiSettingsWindow::LayoutPropsPage::apply() {
	m_layoutList.apply();
}
