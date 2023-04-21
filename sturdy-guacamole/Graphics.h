#pragma once
#include "Singleton.h"

#include <d3d11.h>
#include <filesystem>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace sturdy_guacamole
{
	class Graphics : protected Singleton<Graphics>
	{
	public:
		Graphics();
		~Graphics() = default;

	private:
		void ThrowIfFailed(HRESULT hr, ID3DBlob* errorBlob = nullptr);

		ComPtr<ID3DBlob> CreateShaderBlob(std::filesystem::path csoFilePath);
		ComPtr<ID3D11VertexShader> CreateVertexShader(std::filesystem::path csoFilePath, ID3D11InputLayout** ppInputLayout = nullptr, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs = nullptr, UINT numElements = 0);
		ComPtr<ID3D11PixelShader> CreatePixelShader(std::filesystem::path csoFilePath);
		ComPtr<ID3D11Buffer> CreateVertexBuffer(const void* pVertexData, UINT vertexDataSize);
		ComPtr<ID3D11Buffer> CreateIndexBuffer(const void* pIndexData, UINT indexDataSize);


	};
}
