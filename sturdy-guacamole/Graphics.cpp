#include "Graphics.h"
#include "Dx11Application.h"

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

	// For test
	D3D11_INPUT_ELEMENT_DESC inputElementDescs[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ComPtr<ID3D11InputLayout> inputLayout;
	auto vertexShader = CreateVertexShader(OutDir / L"BasicVS.cso", &inputLayout, inputElementDescs, ARRAYSIZE(inputElementDescs));
	auto pixelShader = CreatePixelShader(OutDir / L"BasicPS.cso");

	ComPtr<ID3D11RasterizerState> rasterizerState;
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = TRUE;

	g_pDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	g_pDeviceContext->RSSetState(rasterizerState.Get());

	// Set the input layout
	g_pDeviceContext->IASetInputLayout(inputLayout.Get());

	// Set the primitive topology
	g_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex shader
	g_pDeviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);

	// Set the pixel shader
	g_pDeviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
}

void sturdy_guacamole::Graphics::ThrowIfFailed(HRESULT hr, ID3DBlob* errorBlob)
{
	if (FAILED(hr))
	{
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		::OutputDebugString(errMsg);

		if (errorBlob)
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

		throw hr;
	}
}

ComPtr<ID3DBlob> sturdy_guacamole::Graphics::CreateShaderBlob(std::filesystem::path csoFilePath)
{
	ComPtr<ID3DBlob> shaderBlob;
	HRESULT hr = ::D3DReadFileToBlob(csoFilePath.c_str(), &shaderBlob);
	ThrowIfFailed(hr);

	return shaderBlob;
}

ComPtr<ID3D11VertexShader> sturdy_guacamole::Graphics::CreateVertexShader(std::filesystem::path csoFilePath, ID3D11InputLayout** ppInputLayout, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements)
{
	ComPtr<ID3DBlob> vertexShaderBlob = CreateShaderBlob(csoFilePath);
	ComPtr<ID3D11VertexShader> vertexShader;

	HRESULT hr{};
	hr = g_pDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(),
		nullptr, &vertexShader);
	ThrowIfFailed(hr);

	if (ppInputLayout)
	{
		hr = g_pDevice->CreateInputLayout(inputElementDescs, numElements,
			vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), ppInputLayout);
		ThrowIfFailed(hr);
	}

	return vertexShader;
}

ComPtr<ID3D11PixelShader> sturdy_guacamole::Graphics::CreatePixelShader(std::filesystem::path csoFilePath)
{
	ComPtr<ID3DBlob> pixelShaderBlob = CreateShaderBlob(csoFilePath);
	ComPtr<ID3D11PixelShader> pixelShader;

	HRESULT hr{};
	hr = g_pDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(),
				nullptr, &pixelShader);
	ThrowIfFailed(hr);

	return pixelShader;
}

ComPtr<ID3D11Buffer> sturdy_guacamole::Graphics::CreateVertexBuffer(const void* pVertexData, UINT vertexDataSize)
{
	D3D11_BUFFER_DESC vertexbufferDesc{};
	vertexbufferDesc.ByteWidth = vertexDataSize;
	vertexbufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = pVertexData;

	ComPtr<ID3D11Buffer> vertexBuffer;
	HRESULT hr = g_pDevice->CreateBuffer(&vertexbufferDesc, &subresourceData, &vertexBuffer);
	ThrowIfFailed(hr);

	return vertexBuffer;
}

ComPtr<ID3D11Buffer> sturdy_guacamole::Graphics::CreateIndexBuffer(const void* pIndexData, UINT indexDataSize)
{
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = indexDataSize;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = pIndexData;
	
	ComPtr<ID3D11Buffer> indexBuffer;
	HRESULT hr = g_pDevice->CreateBuffer(&indexBufferDesc, &subresourceData, &indexBuffer);
	ThrowIfFailed(hr);

	return indexBuffer;
}
