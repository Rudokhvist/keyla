#include "common.h"
#include "application.h"
#include "core.h"
#include "layoutHook.h"
#include "layoutList.h"
#include "settings.h"
#include "trayIcon.h"

#include <algorithm>
using namespace std;

// Remember the layout we switched to in order to allow this change in layoutChanged().
// ExpectedLayout must store a real layout (not an HKL_* contant)
// to be properly compared in layoutChanged().
//
static HKL ExpectedLayout;

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

		// We do not pass HKL_NEXT and manually walk through our layout list
		// because ::setLayout() does not handle HKL_* constants

		std::vector<HKL>::const_iterator it = layoutList::LayoutList.begin();
		const std::vector<HKL>::const_iterator end = layoutList::LayoutList.end();
		for (; it != end; ++it)
			if (*it == ExpectedLayout)
				break;

		assert(it != end);
		if (it == end)
			// Do not change layout in case of error
			return;

		++it;
		if (it == end)
			it = layoutList::LayoutList.begin();

		::setLayout(GetForegroundWindow(), *it);
	}

	void setLayout(unsigned int index) {
		::setLayout(GetForegroundWindow(), layoutList::LayoutList[index] );
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

	void layoutChanged(HWND window, HKL layout) {

			// Get the top-level window containing the specified one
		window = ::GetAncestor(window, GA_ROOT);
		assert(window == GetForegroundWindow());
		if (window != GetForegroundWindow())
			// Ignore
			return;

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
			// If we allowed the change, save and indicate the new layout...
			ExpectedLayout = layout;
			trayIcon::indicateLayout(layout);
		}
		else
			// ...otherwise restore layout
			::setLayout(window, ExpectedLayout);
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