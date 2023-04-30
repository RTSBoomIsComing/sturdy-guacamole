#include "Graphics.h"
#include "Dx11Application.h"
#include "Dx11Helpers.h"

#include <d3dcompiler.h>
#include <comdef.h>

sturdy_guacamole::Graphics::Graphics()
{
	// Get the path of the executable
	WCHAR exeFilePath[MAX_PATH]{};
	::GetModuleFileNameW(NULL, exeFilePath, MAX_PATH);

	// Get the path of Output directory (where the shader files are)
	std::filesystem::path OutDir = exeFilePath;
	OutDir = OutDir.parent_path();

	// Create vertex shader blobs and vertex shaders 
	{
		m_vsBlob.basic = CreateShaderBlob(OutDir / L"BasicVS.cso");
		m_vtxShader.basic = CreateVertexShader(g_pDevice.Get(), m_vsBlob.basic.Get());

	}
	// Set the default vertex shader
	g_pDeviceContext->VSSetShader(m_vtxShader.basic.Get(), nullptr, 0);

	// Create the pixel shader, pixel shader blob is not needed anymore
	{
		auto psBlob = CreateShaderBlob(OutDir / L"BasicPS.cso");
		m_pixShader.basic = CreatePixelShader(g_pDevice.Get(), psBlob.Get());
	}
	// Set the default pixel shader
	g_pDeviceContext->PSSetShader(m_pixShader.basic.Get(), nullptr, 0);


	// Create rasterizer states 
	{
		CD3D11_RASTERIZER_DESC rasterizerDesc{ D3D11_DEFAULT };
		rasterizerDesc.FrontCounterClockwise = TRUE;
		rasterizerDesc.DepthClipEnable = FALSE;
		ThrowIfFailed(g_pDevice->CreateRasterizerState(&rasterizerDesc, &m_rsstate.solid));

		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		ThrowIfFailed(g_pDevice->CreateRasterizerState(&rasterizerDesc, &m_rsstate.wireframe));
	}

	// Set the default rasterizer state
	g_pDeviceContext->RSSetState(m_rsstate.solid.Get());

	// Create depth stencil state
	static ComPtr<ID3D11DepthStencilState> depthStencilState{};
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ D3D11_DEFAULT };
	ThrowIfFailed(g_pDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));
	g_pDeviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
}


