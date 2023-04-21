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

		ComPtr<ID3D11Device> Device{};
		ComPtr<ID3D11DeviceContext> DeviceContext{};
		ComPtr<IDXGISwapChain> SwapChain{};
		ComPtr<ID3D11RenderTargetView> RenderTargetView{};
	};
}

#define g_device sturdy_guacamole::Dx11Application::Get().Device
#define g_deviceContext sturdy_guacamole::Dx11Application::Get().DeviceContext
#define g_swapChain sturdy_guacamole::Dx11Application::Get().SwapChain
#define g_renderTargetView sturdy_guacamole::Dx11Application::Get().RenderTargetView
