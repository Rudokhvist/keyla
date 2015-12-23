#include "common.h"
#include "application.h"
#include "core.h"
#include "layoutHook.h"
#include "layoutList.h"
#include "settings.h"
#include "trayIcon.h"
#include "keyboardhook.h"

#include <algorithm>
using namespace std;

// Remember the layout we switched to in order to allow this change in layoutChanged().
// ExpectedLayout must store a real layout (not an HKL_* contant)
// to be properly compared in layoutChanged().
//
static HKL ExpectedLayout = 0;

// Set layout in particular window
// Function overwrited global variable ExpectedLayout.
//
// window - window
// layout - layout to switch to
//
// Layout must not be HKL_NEXT or any other HKL_* constant
// because it is saved in ExpectedLayout.
//
void setLayout(HWND window, HKL layout) {
	ExpectedLayout = layout;
	layoutHook::setLayout(window, layout);
}

namespace core {

	void nextLayout() {

		size_t sz = settings::Settings.layoutSettings.size();
		assert(sz == layoutList::LayoutList.size());

		size_t i = 0;
		for (; i < sz; ++i)
			if (layoutList::LayoutList[i] == ExpectedLayout)
				break;

		assert(i != sz);
		if (i == sz)
			// Do not change layout in case of error
			return;

		for (size_t j = (i + 1) % sz; j != i; j = (j + 1) % sz) {
			if (settings::Settings.layoutSettings[j].useWhenSwitchingCyclically) {
				::setLayout(GetForegroundWindow(), layoutList::LayoutList[j]);
				return;
			}
		}
	}

	void setLayout(unsigned int index) {
		::setLayout(GetForegroundWindow(), layoutList::LayoutList[index] );
	}

	void activeWindowChanged(HWND activeWindow, HKL layout) {
		if (GetApplication().isActive() && ExpectedLayout != 0 && settings::Settings.globalLayout) {
			// Switch layout in this window to match the global layout
			// We will set tray icon and do other indication in layoutChanged(), which we will certainly reach
			::setLayout(activeWindow, ExpectedLayout);
			return;
		}

		// Save layout and indicate it
		ExpectedLayout = layout;
		trayIcon::indicateLayout(layout);
	}

	void layoutChanged(HWND window, HKL layout) {

			// Get the top-level window containing the specified one
		window = ::GetAncestor(window, GA_ROOT);
		assert(window == GetForegroundWindow());
		if (window != GetForegroundWindow())
			// Ignore
			return;

		bool ret;
		if (!GetApplication().isActive()) {
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
			// If we allowed the change, save and indicate the new layout...
			ExpectedLayout = layout;
			trayIcon::indicateLayout(layout);
		}
		else
			// ...otherwise restore layout
			::setLayout(window, ExpectedLayout);
	}

	bool keyPressed(unsigned int vk, unsigned int modifiers) {
		if (!GetApplication().isActive())
			return false;
		
		HotKey h(vk, modifiers);

		if (settings::Settings.mainHotKey == h) {
			nextLayout();
			return true;
		}

		vector<settings::SettingsStruct::LayoutSettings>::const_iterator begin = settings::Settings.layoutSettings.begin();
		vector<settings::SettingsStruct::LayoutSettings>::const_iterator end = settings::Settings.layoutSettings.end();
		vector<settings::SettingsStruct::LayoutSettings>::const_iterator it = begin;
		for (; it != end; ++it)
			if (it->hotKey == h) {
				setLayout(static_cast<unsigned int>(distance(begin, it)));
				return true;
			}
		return false;
	}

	HKL getLayout() {
		return ExpectedLayout;
	}

	void resetState() {
		keyboardHook::resetState();
	}

}