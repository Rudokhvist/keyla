#include "../common.h"
#include "../res/resource.h"
#include "HotKey.h"

/* static */ const tstring HotKey::Separator = TEXT(" + ");

/* static */ tstring HotKey::keyName(unsigned int vk, bool extended /* = false */) {
	const unsigned int keyNameBufferSize = 64;
	TCHAR keyNameBuffer[keyNameBufferSize] = {0};

	unsigned int code = MapVirtualKey(vk, 0) << 16 | (extended << 24);
	GetKeyNameText(code, keyNameBuffer, keyNameBufferSize);
	return tstring(keyNameBuffer);
}

HotKey::HotKey() {
	clear();
}

HotKey::HotKey(unsigned int vk, unsigned int modifiers) {
	set(vk, modifiers);
}

bool HotKey::operator==(const HotKey & other) const {
	return m_vk == other.m_vk && m_modifiers == other.m_modifiers;
}

void HotKey::set(unsigned int vk, unsigned int modifiers)
{
	if (m_modifiers == modifiers && m_vk == vk) return;
	
	m_modifiers = modifiers;
	m_vk = vk;

	makeText();
}

void HotKey::clear() {
	m_modifiers = 0;
	m_vk = 0;
	m_text.clear();
}

unsigned int HotKey::vk() const {
	return m_vk;
}

unsigned int HotKey::modifiers() const {
	return m_modifiers;
}

const tstring & HotKey::text() const {
	return m_text;
}

void HotKey::loadFromRegistry(HKEY hkey_, const tstring & key) {
	DWORD type;
	DWORD data;
	DWORD dataSz = sizeof(DWORD);

	LPCTSTR value;
	LONG ret;	

	HKEY hkey;
	if (ERROR_SUCCESS != RegCreateKeyEx(hkey_, key.c_str(), 0, 0, 0, KEY_ALL_ACCESS, 0, &hkey, 0)) {
		assert(false);
		return;
	}

	value = TEXT("vk");
	ret = RegQueryValueEx(hkey, value, 0, &type, reinterpret_cast<BYTE *>(&data), &dataSz);
	if (ret != ERROR_SUCCESS || type != REG_DWORD || dataSz != sizeof(DWORD)) {
		data = m_vk;
		RegSetValueEx(hkey, value, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&data), dataSz);
	} else { 
		m_vk = data;
	}

	value = TEXT("modifiers");
	ret = RegQueryValueEx(hkey, value, 0, &type, reinterpret_cast<BYTE *>(&data), &dataSz);
	if (ret != ERROR_SUCCESS || type != REG_DWORD || dataSz != sizeof(DWORD)) {
		data = m_modifiers;
		RegSetValueEx(hkey, value, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&data), dataSz);
	} else {
		m_modifiers = data;
	}

	RegCloseKey(hkey);

	makeText();
}

void HotKey::saveToRegistry(HKEY hkey_, const tstring & key) {

	DWORD data;
	LPCTSTR value;

	HKEY hkey;
	if (ERROR_SUCCESS != RegCreateKeyEx(hkey_, key.c_str(), 0, 0, 0, KEY_ALL_ACCESS, 0, &hkey, 0)) {
		assert(false);
		return;
	}

	value = TEXT("vk");
	data = m_vk;
	RegSetValueEx(hkey, value, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&data), sizeof(data));

	value = TEXT("modifiers");
	data = m_modifiers;
	RegSetValueEx(hkey, value, 0, REG_DWORD, reinterpret_cast<const BYTE *>(&data), sizeof(data));

	RegCloseKey(hkey);
}

void HotKey::makeText() {
	m_text.clear();

	if (m_modifiers & LControl) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_LEFT);
		m_text += keyName(VK_CONTROL);
	}
	if (m_modifiers & LAlt) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_LEFT);
		m_text += keyName(VK_MENU);
	}
	if (m_modifiers & LShift) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_LEFT);
		m_text += keyName(VK_SHIFT);
	}
	if (m_modifiers & RControl) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_RIGHT);
		m_text += keyName(VK_CONTROL);
	}
	if (m_modifiers & RAlt) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_RIGHT);
		m_text += keyName(VK_MENU);
	}
	if (m_modifiers & RShift) {
		if (!m_text.empty()) m_text += Separator;
		m_text += LoadStringLang(IDS_RIGHT);
		m_text += keyName(VK_SHIFT);
	}
    
	if (!m_text.empty()) m_text += Separator;
	m_text += keyName(m_vk, (m_modifiers & Extended) != 0);
}
