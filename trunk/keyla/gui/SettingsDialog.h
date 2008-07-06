#pragma once

#include "Brush.h"
#include "../settings.h"
#include "../../win32pp/Dialog.h"

class SettingsDialog : public CDialog {

	friend class LayoutList;

public:

	// Конструктор
	SettingsDialog();

protected:

	virtual BOOL OnInitDialog();
	virtual BOOL DialogProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	virtual BOOL OnCommand(WPARAM wparam, LPARAM lparam);
	virtual LRESULT OnNotify(WPARAM wparam, LPARAM lparam);

private:

	// Сохранить сделанные изменения
	void apply();

	// Локальная копия настроек
	settings::SettingsStruct m_settings;

	// Кисть для фона элементов управления. Нужна, поскольку
	// фон вкладок отличается от фона диалогового окна
	static Brush BackgroundBrush;
	
};