#include "../win32xx/wxx_wincore.h"
#include "common.h"
#include "application.h"
#include "layoutHook.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "trayIcon.h"


#include <list>
using namespace std;

namespace {

class MainWindow : public CWnd {
private:

	typedef list<mainWindow::HandlerProc> Handlers;
	Handlers m_handlers;

public:

	void AddMessageHandler(mainWindow::HandlerProc handler) {
		m_handlers.push_back(handler);
	}

protected:

	virtual void PreRegisterClass(WNDCLASS & wc) {
		CWnd::PreRegisterClass(wc);

		// Use partucular window class to be able to find a running application instance easily
		wc.lpszClassName = TEXT("keyla main window");
	}

	virtual void PreCreate(CREATESTRUCT & cs) {
		// Window only handles messages. It does not interact with user, so hide it.
		cs.style &= ~WS_VISIBLE;

		// NOTE: CWnd overwrites our changes in cs.style if it is zero. So let it be non-zero.
		cs.style |= WS_BORDER;

		// NOTE: Due to CWnd's implementation we first need to change cs
		// and after that call the base class member
		CWnd::PreCreate(cs);
	}

	virtual int OnCreate(LPCREATESTRUCT pcs) {
		CWnd::OnCreate(pcs);

		trayIcon::create(GetHwnd());
		layoutHook::create(GetHwnd());
		return 0;
	}

	virtual LRESULT WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {

		if (message == WM_DESTROY) {
			// Quit the application
			PostQuitMessage(0);
		}

		LRESULT ret = 0;
		Handlers::const_iterator it = m_handlers.begin();
		const Handlers::const_iterator end = m_handlers.end();
		for (; it != end; ++it) {
			if ((*it)(window, message, wparam, lparam, &ret))
				return ret;
		}

		return CWnd::WndProc(message, wparam, lparam);
	}

} MyMainWindow;

}

namespace mainWindow {

	void create() {
		MyMainWindow.Create();
	}

	void addMessageHandler(HandlerProc handler) {
		return MyMainWindow.AddMessageHandler(handler);
	}

	void destroy() {
		MyMainWindow.Destroy();
	}

}