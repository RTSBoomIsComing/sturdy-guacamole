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
		int m_width{ 1280 };
		int m_height{ 960 };
	};
}