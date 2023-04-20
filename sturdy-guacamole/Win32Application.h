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

		HWND GetWindowHandle() const { return m_hWnd; }
	};
}

