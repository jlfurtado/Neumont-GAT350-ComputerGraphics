#ifndef KEYBOARD_H
#define KEYBOARD_H

// Justin Furtado
// 7/10/2016
// Keyboard.h
// Calculates pressed and released states for keys

struct Key
{
	int vKey;
	bool lastDownState;
	bool downState;
};

struct KeyToggle
{
	bool fireOnPress;
	bool *pToggle;
	int vKey;
};

class Keyboard
{
public:
	static Keyboard *pFirst;

	// ctor/dtor
	Keyboard();
	~Keyboard();

	// public methods
	bool KeyWasPressed(int vKey);
	bool KeyWasReleased(int vKey);
	bool KeyIsDown(int vKey);
	bool KeyIsUp(int vKey);
	bool AddToggle(int vKey, bool *pToggle, bool onFirePress = true);
	void Update(float dt);
	bool AddKey(int vKey);
	bool AddKeys(const char *keys);

private:
	// Methods
	void UpdateKeys();
	void UpdateToggles();

	// data
	static const int MAX_KEY = 256;
	KeyToggle m_keyToggles[MAX_KEY]{ 0 };
	Key m_keyStates[MAX_KEY]{ 0 };
	int m_keys[MAX_KEY]{ 0 };
	int m_nextKey{ 0 };
	int m_numToggles{ 0 };
};

#endif // ifndef KEYBOARD_H