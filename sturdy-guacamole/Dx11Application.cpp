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
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &SwapChain, &Device, &featureLevel, &DeviceContext);
	
	if (hr == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &SwapChain, &Device, &featureLevel, &DeviceContext);
	
	if (FAILED(hr))
		throw std::runtime_error("Failed to create device and swap chain.");

	// Create render target;
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer from swap chain.");

	hr = Device->CreateRenderTargetView(backBuffer.Get(), NULL, &RenderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create render target view.");
}

void sturdy_guacamole::Dx11Application::ResizeRenderTarget()
{
	// Release render target view
	RenderTargetView = nullptr;

	// Resize swap chain
	SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	// Create a render target view
	ComPtr<ID3D11Texture2D> backBuffer;
	SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &RenderTargetView);
}
