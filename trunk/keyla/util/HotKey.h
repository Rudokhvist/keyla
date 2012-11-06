//
// Helper module HOTKEY provides class HotKey
//

#pragma once

// Class to store a shortcut
class HotKey {
public:

	// Modifiers
	enum Modifiers {
		LControl  = 1,
		LAlt      = 2,
		LShift    = 4,
		LWin      = 64,
		RControl  = 8,
		RAlt      = 16,
		RShift    = 32,
		RWin      = 256,
		Extended  = 128, // Flag of "extended key"
	};

	// Get key name
	// vk - virtual key code
	// extended - whether the key is "extended"
	static tstring keyName(unsigned int vk, bool extended = false);


	// Default constructor
	HotKey();

	// Constructor
	// vk - virtual key code
	// modifiers - bitwise OR of Modifiers
	HotKey(unsigned int vk, unsigned int modifiers);


	bool operator==(const HotKey & other) const;


	// Set shortcut
	// vk - virtual key code
	// modifiers - bitwise OR of Modifiers
	void set(unsigned int vk, unsigned int modifiers);

	// Clear shortcut
	void clear();


	// Get virtual key code
	unsigned int vk() const;

	// Get modifiers (bitwise OR of Modifiers)
	unsigned int modifiers() const;

	// Get textual representation of the shortcut
	const tstring & text() const;
	

	// Load data from registry
	// hkey - registry key or hive
	// key - subkey
	void loadFromRegistry(HKEY hkey, const tstring & key);

	// Save data to registry
	// hkey - registry key or hive
	// key - subkey
	void saveToRegistry(HKEY hkey, const tstring & key);

private:

	// Fill m_text accorging to m_vk and m_modifiers
	void makeText();
	
	// Virtual key code
	unsigned int m_vk;

	// Bitwise OR of Modifiers
	unsigned int m_modifiers;

	// Textual representation of the shortcut
	// NOTE: Must be updated when m_modifiers and/or m_vk changes
	tstring m_text;

	// Separator used in textual representation of a shortcut
	static const tstring Separator;
};
