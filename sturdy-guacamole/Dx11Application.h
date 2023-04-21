#pragma once

#include "Singleton.h"

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace sturdy_guacamole
{
	class Dx11Application : protected Singleton<Dx11Application>
	{
	public:
		Dx11Application(HWND hWnd);
		~Dx11Application() = default;

	private:
		ComPtr<ID3D11Device> m_device{};
		ComPtr<ID3D11DeviceContext> m_deviceContext{};
		ComPtr<IDXGISwapChain> m_swapChain{};
		ComPtr<ID3D11RenderTargetView> m_renderTargetView{};

	public:
		static ID3D11Device* GetDevice() { return GetInstance().m_device.Get(); }
		static ID3D11DeviceContext* GetDeviceContext() { return GetInstance().m_deviceContext.Get(); }
		static IDXGISwapChain* GetSwapChain() { return GetInstance().m_swapChain.Get(); }
		static ID3D11RenderTargetView* GetRenderTargetView() { return GetInstance().m_renderTargetView.Get(); }

	public:
		static void ResizeRenderTarget();
	};
}

#define g_pDevice sturdy_guacamole::Dx11Application::GetDevice()
#define g_pDeviceContext sturdy_guacamole::Dx11Application::GetDeviceContext()
#define g_pSwapChain sturdy_guacamole::Dx11Application::GetSwapChain()
#define g_pRenderTargetView sturdy_guacamole::Dx11Application::GetRenderTargetView()
