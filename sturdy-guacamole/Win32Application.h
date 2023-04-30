#pragma once
#include "Singleton.h"
#include <Windows.h>

namespace sturdy_guacamole
{
	class Win32Application : public Singleton<Win32Application>
	{
	private:
		LPCWSTR m_className = L"sturdy-guacamole";
	public:
		Win32Application();
		~Win32Application();

	public:
		HWND m_hWnd{};
		UINT m_width{ 1280u };
		UINT m_height{ 960u };
	};

	using Win32App = sturdy_guacamole::Win32Application;
}
