#include "Keyboard.hpp"

#include <Windows.h>

#include <array>
#include <atomic>

namespace YimMenu::Input
{
	namespace
	{
		using KeyboardHandler = void (*)(DWORD, WORD, BYTE, BOOL, BOOL, BOOL, BOOL);
		using KeyboardHandlerRegister = void (*)(KeyboardHandler);
		using KeyboardHandlerUnregister = void (*)(KeyboardHandler);

		std::array<std::atomic<bool>, 256> s_KeyStates{};
		KeyboardHandlerUnregister s_UnregisterHandler = nullptr;
		bool s_UsingScriptHook = false;

		void HandleKeyboard(DWORD key, WORD, BYTE, BOOL, BOOL, BOOL, BOOL is_up_now)
		{
			if (key < s_KeyStates.size())
			{
				s_KeyStates[key].store(!is_up_now, std::memory_order_relaxed);
			}
		}
	}

	void Initialize()
	{
		if (s_UsingScriptHook)
		{
			return;
		}

		HMODULE script_hook = GetModuleHandleA("ScriptHookRDR2.dll");
		if (!script_hook)
		{
			return;
		}

		auto register_handler = reinterpret_cast<KeyboardHandlerRegister>(GetProcAddress(script_hook, "keyboardHandlerRegister"));
		auto unregister_handler = reinterpret_cast<KeyboardHandlerUnregister>(GetProcAddress(script_hook, "keyboardHandlerUnregister"));
		if (!register_handler || !unregister_handler)
		{
			return;
		}

		register_handler(HandleKeyboard);
		s_UnregisterHandler = unregister_handler;
		s_UsingScriptHook = true;
	}

	void Shutdown()
	{
		if (s_UsingScriptHook && s_UnregisterHandler)
		{
			s_UnregisterHandler(HandleKeyboard);
		}

		s_UnregisterHandler = nullptr;
		s_UsingScriptHook = false;
	}

	bool IsKeyDown(int key)
	{
		if (key < 0 || key >= static_cast<int>(s_KeyStates.size()))
		{
			return false;
		}

		if (s_UsingScriptHook)
		{
			return s_KeyStates[key].load(std::memory_order_relaxed);
		}

		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}
}
