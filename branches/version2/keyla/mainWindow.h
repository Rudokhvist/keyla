//
// Module MAIN_WINDOW provides some functions to control the main window.
// The main window does not interact with user, but does handle some messages from the tray icon.
//

#pragma once

namespace mainWindow {

	// Initialize.
	void create();

	// Add a message handling function
	typedef bool (*HandlerProc)(HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT * ret);
	void addMessageHandler(HandlerProc handler);

	// Deinitialize.
	void destroy();
}