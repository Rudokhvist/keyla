#include "common.h"

#include "gui/SettingsDialog.h"
#include "settingsWindow.h"

static SettingsDialog MySettingsDialog;

namespace settingsWindow {

	void show() {
		MySettingsDialog.DoModal();
	}

}