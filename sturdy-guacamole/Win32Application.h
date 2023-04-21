#pragma once
#include "Singleton.h"
#include <Windows.h>

namespace sturdy_guacamole
{
	class Win32Application : public Singleton<Win32Application>
	{
	private:
		HWND m_hWnd;
		LPCWSTR m_className = L"sturdy-guacamole";
	public:
		Win32Application();
		~Win32Application();

	public:
		static HWND GetWindowHandle() { return GetInstance().m_hWnd; }
	};
}

#define g_hWnd sturdy_guacamole::Win32Application::GetWindowHandle()