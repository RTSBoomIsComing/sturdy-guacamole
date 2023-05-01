#include "Graphics.h"
#include "Dx11Application.h"
#include "Dx11Helpers.h"
#include "Win32Application.h"

#include <d3dcompiler.h>
#include <comdef.h>

sturdy_guacamole::Graphics::Graphics()
{
	// Get the path of the executable
	WCHAR exeFilePath[MAX_PATH]{};
	::GetModuleFileNameW(NULL, exeFilePath, MAX_PATH);

	// Get the path of Output directory where the compiled shader objects (.cso) are
	std::filesystem::path OutDir = exeFilePath;
	OutDir = OutDir.parent_path();

	// Set the default vertex shader
	this->InitVertexShaders(OutDir);
	g_pDeviceContext->VSSetShader(m_vtxShader.basic.Get(), nullptr, 0);

	// Set the default pixel shader
	this->InitPixelShaders(OutDir);
	g_pDeviceContext->PSSetShader(m_pixShader.basic.Get(), nullptr, 0);

	// Set the default rasterizer state
	this->InitRasterizerStates();
	g_pDeviceContext->RSSetState(m_rsstate.wireframe.Get());

	// Set the default depth stencil state
	this->InitDepthStencilStates();
	g_pDeviceContext->OMSetDepthStencilState(m_dsstate.basic.Get(), 0);

	// Set the default rtview and dsview
	this->InitRenderTargetViews();
	this->InitDepthStencilViews();
	g_pDeviceContext->OMSetRenderTargets(1, m_rtview.main.GetAddressOf(), m_dsview.main.Get());

	// Set the default viewport
	CD3D11_VIEWPORT viewport{ 0.0F, 0.0F, static_cast<float>(Win32App::Get().m_width), static_cast<float>(Win32App::Get().m_height)};
	g_pDeviceContext->RSSetViewports(1, &viewport);
}

void sturdy_guacamole::Graphics::InitVertexShaders(std::filesystem::path baseDir)
{
	// Create basic vertex shader
	auto vsBlob = CreateShaderBlob(baseDir / L"BasicVS.cso");
	m_vtxShader.basic_blob = vsBlob;
	ThrowIfFailed(g_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vtxShader.basic));
}

void sturdy_guacamole::Graphics::InitPixelShaders(std::filesystem::path baseDir)
{
	// Create basic pixel shader
	auto psBlob = CreateShaderBlob(baseDir / L"BasicPS.cso");
	ThrowIfFailed(g_pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixShader.basic));
}

void sturdy_guacamole::Graphics::InitGeometryShaders(std::filesystem::path baseDir)
{
	// Create normal geometry shader
	auto gsBlob = CreateShaderBlob(baseDir / L"NormalGS.cso");
	ThrowIfFailed(g_pDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &m_geoShader.normal));
}

void sturdy_guacamole::Graphics::InitRasterizerStates()
{
	// Create default rasterizer desciption
	CD3D11_RASTERIZER_DESC rasterizerDesc{ D3D11_DEFAULT };
	rasterizerDesc.FrontCounterClockwise = TRUE;

	// Create rasterizer state that draw solid polygons
	ThrowIfFailed(g_pDevice->CreateRasterizerState(&rasterizerDesc, &m_rsstate.solid));

	// Create rasterizer state that draw wireframe of polygons
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	ThrowIfFailed(g_pDevice->CreateRasterizerState(&rasterizerDesc, &m_rsstate.wireframe));
}

void sturdy_guacamole::Graphics::InitDepthStencilStates()
{
	// Create default depth stencil desciption
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ D3D11_DEFAULT };

	// Create depth stencil state that pass depth test and write to depth buffer
	ThrowIfFailed(g_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_dsstate.basic));
}

void sturdy_guacamole::Graphics::InitRenderTargetViews()
{
	// Get the back buffer
	ComPtr<ID3D11Texture2D> backBuffer{};
	ThrowIfFailed(g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

	// Create the main render target view
	ThrowIfFailed(g_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtview.main));
}

void sturdy_guacamole::Graphics::InitDepthStencilViews()
{
	// Create the depth stencil texture
	ComPtr<ID3D11Texture2D> dsBuffer{};
	CD3D11_TEXTURE2D_DESC dsBufDesc{ DXGI_FORMAT_D24_UNORM_S8_UINT, Win32App::Get().m_width, Win32App::Get().m_height, 
		1u, 0u, D3D11_BIND_DEPTH_STENCIL };
	ThrowIfFailed(g_pDevice->CreateTexture2D(&dsBufDesc, nullptr, &dsBuffer));

	// Create the main depth stencil view
	CD3D11_DEPTH_STENCIL_VIEW_DESC dsviewDesc{ D3D11_DSV_DIMENSION_TEXTURE2D };
	ThrowIfFailed(g_pDevice->CreateDepthStencilView(dsBuffer.Get(), &dsviewDesc, &m_dsview.main));
}