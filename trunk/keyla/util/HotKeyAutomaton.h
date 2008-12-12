//
// To understand this, look at the `hotkey.png` file in the `diagrams` folder
//

#pragma once

#include "hotkey.h"

class HotKeyAutomaton {
public:
	inline HotKeyAutomaton() : m_modifiers(0) {}
	inline void initialize() { m_modifiers = 0; m_state = StateA; }

	void postEvent(WPARAM wparam, LPARAM lparam);

	inline virtual ~HotKeyAutomaton() {}

protected:
	inline virtual void callbackNoop() {}
	inline virtual void callbackProgress(const HotKey & hotkey) {}
	inline virtual void callbackHotKey(const HotKey & hotkey) {}

private:
	void updateCommandKeys(bool pressed, unsigned int vk);

	HotKey getHotKeyWithSymbolKey(unsigned int vk, bool extended) const;

private:
	unsigned int m_modifiers;

	enum State { StateA, StateB, StateC };
	enum Event { P_smb, P_com, R_smb, R_com };

	State m_state;
};