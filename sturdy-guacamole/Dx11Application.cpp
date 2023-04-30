#include "Dx11Application.h"
#include <stdexcept>

sturdy_guacamole::Dx11Application::Dx11Application(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	UINT createDeviceFlags{};
#if defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2]{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	
	HRESULT hr{};
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, &featureLevel, &m_deviceContext);
	
	if (hr == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_swapChain, &m_device, &featureLevel, &m_deviceContext);
	
	if (FAILED(hr))
		throw std::runtime_error("Failed to create device and swap chain.");

	// Create render target;
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer from swap chain.");

	hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create render target view.");

	// TODO : create depth stencil view 
	CD3D11_TEXTURE2D_DESC textureDesc{ DXGI_FORMAT_D24_UNORM_S8_UINT, 800, 600, 1, 1, D3D11_BIND_DEPTH_STENCIL };
	//m_device->CreateDepthStencilView()
}

void sturdy_guacamole::Dx11Application::ResizeRenderTarget(UINT newWidth, UINT newHeight)
{
	// Release render target view
	Get().m_renderTargetView = nullptr;

	// Resize swap chain
	m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	// Create a render target view
	ComPtr<ID3D11Texture2D> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &Get().m_renderTargetView);

	// Set viewport
	CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, static_cast<float>(newWidth), static_cast<float>(newHeight) };
	g_pDeviceContext->RSSetViewports(1, &viewport);
}
