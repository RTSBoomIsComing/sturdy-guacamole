#include "Win32Application.h"

sturdy_guacamole::Win32Application::Win32Application()
{
	// Create application window
	WNDCLASSEXW wc{ sizeof(wc), CS_CLASSDC, DefWindowProcW, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, m_className, NULL };
	::RegisterClassExW(&wc);
	
	RECT rect{ 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	int adjusted_width{ rect.right - rect.left };
	int adjusted_height{ rect.bottom - rect.top };
	
	m_hWnd = ::CreateWindowW(wc.lpszClassName, L"sturdy guacamole", WS_OVERLAPPEDWINDOW, 100, 30, 
		adjusted_width, adjusted_height, NULL, NULL, wc.hInstance, NULL);



	// Show the window
	::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hWnd);
}

sturdy_guacamole::Win32Application::~Win32Application()
{
	::DestroyWindow(m_hWnd);
	::UnregisterClassW(m_className, GetModuleHandle(NULL));
}
