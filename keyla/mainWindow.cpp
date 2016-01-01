#include "../win32xx/wxx_wincore.h"
#include "common.h"
#include "application.h"
#include "layoutHook.h"
#include "mainWindow.h"
#include "res/resource.h"
#include "trayIcon.h"


#include <list>
using namespace std;

//namespace {

	void  MainWindow::AddMessageHandler(HandlerProc handler) {
		m_handlers.push_back(handler);
	}

	void MainWindow::PreRegisterClass(WNDCLASS & wc) {
		CWnd::PreRegisterClass(wc);

		// Use partucular window class to be able to find a running application instance easily
		wc.lpszClassName = TEXT("keyla main window");
	}

	void MainWindow::PreCreate(CREATESTRUCT & cs) {
		// Window only handles messages. It does not interact with user, so hide it.
		cs.style &= ~WS_VISIBLE;

		// NOTE: CWnd overwrites our changes in cs.style if it is zero. So let it be non-zero.
		cs.style |= WS_BORDER;

		// NOTE: Due to CWnd's implementation we first need to change cs
		// and after that call the base class member
		CWnd::PreCreate(cs);
	}

	int MainWindow::OnCreate(LPCREATESTRUCT pcs) {
		CWnd::OnCreate(pcs);

		trayIcon::create(GetHwnd());
		layoutHook::create(GetHwnd());
		return 0;
	}

	LRESULT MainWindow::WndProc(UINT message, WPARAM wparam, LPARAM lparam) {
		HWND window = m_hWnd;
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



//}
/*
namespace mainWindow {


	void create() {
		MyMainWindow.Create();
	}

	void addMessageHandler(HandlerProc handler) {
		return MyMainWindow.AddMessageHandler(handler);
	}

	void destroy() {
		MyMainWindow.Destroy();
		::PostQuitMessage(0);
	}
*/
//}