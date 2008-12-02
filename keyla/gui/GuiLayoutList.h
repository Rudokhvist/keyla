#pragma once

#include "GuiHotKey.h"
#include "../settings.h"
#include "../../win32xx/WinCore.h"

#include <vector>

class GuiLayoutList;

//
// Helper class of a control to choose shortcut for any layout in the list
//
class HotKeyEditDelegate : public GuiHotKey {
public:

	// Default contstructor
	HotKeyEditDelegate();

	// Override GuiHotKey::Create, because we need GuiLayoutList &, not HWND
	HWND Create(GuiLayoutList & layoutList);

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// The list of layouts that owns this HotKeyEditDelegate
	GuiLayoutList * m_layoutList;
};

//
// Class of a control to display list of layouts and edit shortcuts for any of them
//
// Control must be Attach'ed to an existing control
// Be sure that ListView's type is Report
//
class GuiLayoutList : public CWnd{
public:

	// Default contructor
	GuiLayoutList();

	// Initialize control's contents. update() is called automatically
	void initialize();

	// Update control's contents
	void update();

	// Apply changes (i.e., copy them to the global settings)
	void apply();

	// This method gets called from m_delegate when it looses focus
	// We save the shortcut and hide the delegate
	void delegateDeactivated();

protected:

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Local copy of shortcuts for layouts
	std::vector<settings::SettingsStruct::LayoutSettings> m_layoutSettings;

	// Whether the delegate is active
	bool m_delegateActive;

	// Helper control to choose shortcut for any layout in the list
	HotKeyEditDelegate m_delegate;
	
	// Row where the delegate is placed or 0 if it is not currently active
	unsigned int m_delegateRow;
};