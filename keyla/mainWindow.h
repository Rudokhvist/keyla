//
// Module MAIN_WINDOW provides some functions to control the main window.
// The main window does not interact with user, but does handle some messages from the tray icon.
//

#pragma once

#include "../win32xx/wxx_wincore.h"
#include <list>
using namespace std;

typedef bool (*HandlerProc)(HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT * ret);

class MainWindow : public CWnd {
private:

	typedef list<HandlerProc> Handlers;
	Handlers m_handlers;

public:
	void AddMessageHandler(HandlerProc handler);

protected:

	virtual void PreRegisterClass(WNDCLASS & wc);

	virtual void PreCreate(CREATESTRUCT & cs);
	virtual int OnCreate(LPCREATESTRUCT pcs);

	virtual LRESULT WndProc(UINT message, WPARAM wparam, LPARAM lparam);
};

/*namespace mainWindow {

	// Initialize.
	void create();

	// Add a message handling function
	typedef bool (*HandlerProc)(HWND window, UINT message, WPARAM wparam, LPARAM lparam, LRESULT * ret);
	void addMessageHandler(HandlerProc handler);

	// Deinitialize.
	void destroy();
}*/