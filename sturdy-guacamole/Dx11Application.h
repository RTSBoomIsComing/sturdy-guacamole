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

	private:
		ComPtr<ID3D11Device> Device{};
		ComPtr<ID3D11DeviceContext> DeviceContext{};
		ComPtr<IDXGISwapChain> SwapChain{};
		ComPtr<ID3D11RenderTargetView> RenderTargetView{};

	public:
		ID3D11Device* GetDevice() { return Device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() { return DeviceContext.Get(); }
		IDXGISwapChain* GetSwapChain() { return SwapChain.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() { return RenderTargetView.Get(); }

	public:
		void ResizeRenderTarget();
	};
}

#define g_dx11App sturdy_guacamole::Dx11Application::GetInstance()
#define g_pDevice sturdy_guacamole::Dx11Application::GetInstance().GetDevice()
#define g_pDeviceContext sturdy_guacamole::Dx11Application::GetInstance().GetDeviceContext()
#define g_pSwapChain sturdy_guacamole::Dx11Application::GetInstance().GetSwapChain()
#define g_pRenderTargetView sturdy_guacamole::Dx11Application::GetInstance().GetRenderTargetView()
