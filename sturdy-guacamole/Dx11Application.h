#pragma once

#include "Singleton.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace sturdy_guacamole
{
	class Dx11Application : public Singleton<Dx11Application>
	{
	public:
		Dx11Application(HWND hWnd);
		~Dx11Application() = default;

		ComPtr<ID3D11Device> m_device{};
		ComPtr<ID3D11DeviceContext> m_deviceContext{};
		ComPtr<IDXGISwapChain> m_swapChain{};
	};

	using Dx11App = sturdy_guacamole::Dx11Application;
}

#define g_pDevice sturdy_guacamole::Dx11Application::Get().m_device
#define g_pDeviceContext sturdy_guacamole::Dx11Application::Get().m_deviceContext
#define g_pSwapChain sturdy_guacamole::Dx11Application::Get().m_swapChain

