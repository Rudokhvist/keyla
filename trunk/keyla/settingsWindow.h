//
// Module SETTINGS_WINDOW provides some functions to control the settings window
//

#pragma once

namespace settingsWindow {

	// Show the settings window
	void show();

	// Get to know, whether the settings window is active
	bool isShown();

	// Flash active settings window if any
	void flash();
}