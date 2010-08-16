//
// Module ACTIVE_WINDOW_HOOK keeps track of active window
// When user activates other window, module reports the CORE module about it
//

#pragma once

namespace activeWindowHook {

	// Begin keeping track of active window
	void create();

	// Stop keeping track of active window
	void destroy();
}