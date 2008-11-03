#include "common.h"
#include "application.h"
#include "core.h"
#include "layoutList.h"
#include "settings.h"
#include "trayIcon.h"

#include <algorithm>
using namespace std;

// Remember the layout we switched to in order to allow this change in layoutChanged()
static HKL ExpectedLayout;

// Set layout in particular window
// Function overwrited global variable ExpectedLayout.
//
// window - window
// layout - layout to switch to
//
void setLayout(HWND window, HKL layout) {
	ExpectedLayout = layout;
	PostMessage(window, WM_INPUTLANGCHANGEREQUEST, 0, reinterpret_cast<LPARAM>(layout));
}

namespace core {

	void nextLayout() {
		::setLayout(GetForegroundWindow(), reinterpret_cast<HKL>(HKL_NEXT));
	}

	void setLayout(unsigned int index) {
		::setLayout(GetForegroundWindow(), layoutList::GuiLayoutList[index] );
	}

	void activeWindowChanged(HWND activeWindow, HKL layout) {
		if (Application::GetApp()->isActive() && ExpectedLayout != 0 && settings::Settings.globalLayout) {
			// Switch layout in this window to match the global layout
			// We will set tray icon and do other indication in layoutChanged(), which we will certainly reach
			::setLayout(activeWindow, ExpectedLayout);
			return;
		}

		// Save layout and indicate it
		ExpectedLayout = layout;
		trayIcon::indicateLayout(layout);
	}

	bool layoutChanged(HKL layout) {
		bool ret;
		if (!Application::GetApp()->isActive()) {
			// Permit layout change
			ret = false;
		}
		else if (layout == ExpectedLayout) {
			// Permit layout change
			ret = false;
		}
		else {			
			// Otherwise follow the settings
			ret = settings::Settings.skipSystemHotKey;
		}
		if (ret == false) {
			// If we permitted layout change, indicate new layout
			trayIcon::indicateLayout(layout);
		}
		return ret;
	}

	bool keyPressed(unsigned int vk, unsigned int modifiers) {
		if (!Application::GetApp()->isActive())
			return false;
		
		HotKey h(vk, modifiers);
		if (settings::Settings.mainHotKey == h) {
			nextLayout();
			return true;
		}

		vector<HotKey>::const_iterator begin = settings::Settings.layoutHotKeys.begin();
		vector<HotKey>::const_iterator end = settings::Settings.layoutHotKeys.end();
		vector<HotKey>::const_iterator it = find(begin, end, h);
		if (it != end) {
			setLayout(static_cast<unsigned int>(distance(begin, it)));
			return true;
		}

		return false;
	}

	HKL getLayout() {
		return ExpectedLayout;
	}
}