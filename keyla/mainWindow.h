//
// Module MAIN_WINDOW provides some functions to control the main window.
// The main window does not interact with user, but does handle some messages from the tray icon.
//

#pragma once

namespace mainWindow {

	// Message identifier used by the tray icon
	static const unsigned int TrayIconMessage = WM_USER + 1;

	// Identifier of the only tray icon 
	static const unsigned int TrayIconId = 0;

	void create();

}