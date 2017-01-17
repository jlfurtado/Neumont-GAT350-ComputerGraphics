#include "Keyboard.h"
#include "MyWindow.h"
#include "GameLogger.h"

// Justin Furtado
// 7/10/2016
// Keyboard.cpp
// Calculates pressed and released states for keys

namespace Engine
{
	Keyboard *Keyboard::pFirst = nullptr;

	Keyboard::Keyboard()
	{
		if (pFirst == nullptr) { pFirst = this; }
	}

	Keyboard::~Keyboard()
	{
	}

	bool Keyboard::KeyWasPressed(int vKey)
	{
		return m_keyStates[vKey].downState && !m_keyStates[vKey].lastDownState;
	}

	bool Keyboard::KeyWasReleased(int vKey)
	{
		return !m_keyStates[vKey].downState && m_keyStates[vKey].lastDownState;
	}

	bool Keyboard::KeyIsDown(int vKey)
	{
		return m_keyStates[vKey].downState;
	}

	bool Keyboard::KeyIsUp(int vKey)
	{
		return !m_keyStates[vKey].downState;
	}

	bool Keyboard::AddToggle(int vKey, bool * pToggle, bool onFirePress)
	{
		if (vKey < 0 || vKey > MAX_KEY) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Invalid key [%d] was entered for AddToggle!\n", vKey); return false; }
		if (!pToggle) { Engine::GameLogger::Log(Engine::MessageType::Error, "Error in AddToggle! Can't toggle nullptr!\n"); return false; }

		if (!AddKey(vKey)) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Could not add toggle, failed to add key [%d]!\n", vKey); return false; }
		m_keyToggles[m_numToggles++] = { onFirePress, pToggle, vKey };
		return true;
	}

	void Keyboard::Update(float /*dt*/)
	{
		UpdateKeys();
		UpdateToggles();
	}

	bool Keyboard::AddKey(int vKey)
	{
		if (vKey > MAX_KEY || vKey < 0) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Invalid key [%d] was entered for AddKey!\n", vKey); return false; }

		m_keys[m_nextKey++] = vKey;
		m_keyStates[vKey] = { vKey, KeyIsDown(vKey), KeyIsDown(vKey) };
		return true;
	}

	bool Keyboard::AddKeys(const char *keys)
	{
		if (!keys) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Invalid keys were entered for AddKeys!\n"); return false; }

		for (int i = 0; *(keys + i); ++i)
		{
			if (!AddKey(*(keys + i))) { Engine::GameLogger::Log(Engine::MessageType::Warning, "Failed to add key [%d]!\n", *(keys + i)); return false; }
		}

		return true;
	}

	void Keyboard::UpdateKeys()
	{
		// update keys
		for (int i = 0; i < m_nextKey; ++i)
		{
			int vKey = m_keys[i];
			m_keyStates[vKey].lastDownState = m_keyStates[vKey].downState;
			m_keyStates[vKey].downState = (GetAsyncKeyState(m_keyStates[vKey].vKey) != 0);
		}
	}

	void Keyboard::UpdateToggles()
	{
		// update toggles
		for (int i = 0; i < m_numToggles; ++i)
		{
			if (!m_keyToggles[i].pToggle) { continue; }

			if (m_keyToggles[i].fireOnPress ? KeyWasPressed(m_keyToggles[i].vKey) : KeyWasReleased(m_keyToggles[i].vKey))
			{
				*(m_keyToggles[i].pToggle) = !(*(m_keyToggles[i].pToggle));
			}
		}
	}
}