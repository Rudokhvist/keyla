//
// Module SETTINGS can read program settings from the registry and save them there
//

#pragma once

#include "gui/Icon.h"
#include "util/HotKey.h"

#include <vector>

namespace settings {

	//
	// Settings are stored here
	//
	extern struct SettingsStruct {
		SettingsStruct();

		// Shortcut to switch to next layout
		HotKey mainHotKey;
		// Forbid external change of layout (via the language panel or using system shortcut)
		bool skipSystemHotKey;
		// Whether to use single global layout
		bool globalLayout;
		// Shortcuts to switch to particular layouts
		std::vector<HotKey> layoutHotKeys;
		// Program icon
		Icon mainIcon;
	} Settings;

	// Load program settings from the registry
	void load();

	// Save program settings to the registry
	void save();
}