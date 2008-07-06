#include "common.h"

#include "gui/SettingsDialog.h"
#include "mainWindow.h"
#include "settingsWindow.h"

static SettingsDialog MySettingsDialog;

namespace settingsWindow {

	void show() {
		MySettingsDialog.DoModal();
	}

	bool isShown() {
		return IsWindowVisible(MySettingsDialog) != 0;
	}

}