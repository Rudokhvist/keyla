#pragma once

#include "Menu.h"
#include "../util/HotKey.h"
#include "../../win32xx/WinCore.h"

//
// Class of a control to choose a key binding
//
// A control can be Create'd of Attach'ed to an existing control.
// A control is initially invisible when it is Create'd.
// If you use Attach, be sure that the control is disabled.
//
class GuiHotKey : public CWnd{
public:

	// Default constructor
	GuiHotKey();

	// Set shortcut
	void setHotKey(const HotKey & hotKey);

	// Set shortcut
	// Parameters' description can be found near HotKey::set declaration
	void setHotKey(unsigned int vk, unsigned int modifiers);

	// Clear shortcut
	void clearHotKey();

	// Get shortcut
	const HotKey & hotKey() const;

protected:

	virtual void PreCreate(CREATESTRUCT & cs);
	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

private:

	// Initialize the control
	void initialize();

	// Used to determine whether to initialize()
	HWND m_lastInitializedHwnd;

	// Shortcut itself
	HotKey m_hotKey;


	// The control that is currently focused or NULL
	static GuiHotKey * ActiveInstance;


	// Keyboard hook procedure. It must be WH_KEYBOARD_LL in order to
	// catch all shortcuts, including ones used by Windows (e.g., Win-D)
	static LRESULT CALLBACK keyboardHook(int code, WPARAM wparam, LPARAM lparam);

	// Keyboard hook handle
	static HHOOK KeyboardHook;

	// Modifiers (see HotKey::Modifiers) currently pressed
	// Used when the keyboard hook is active
	static unsigned int Modifiers;

	// Context menu
	static Menu ContextMenu;
};