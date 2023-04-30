#include "Dx11Helpers.h"

#include <d3dcompiler.h>
#include <comdef.h>
namespace sturdy_guacamole
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			_com_error err(hr);
			LPCTSTR errMsg = err.ErrorMessage();
			::OutputDebugString(errMsg);
			throw hr;
		}
	}

	ComPtr<ID3DBlob> CreateShaderBlob(std::filesystem::path csoPath)
	{
		ComPtr<ID3DBlob> shaderBlob{};
		HRESULT hr = ::D3DReadFileToBlob(csoPath.c_str(), &shaderBlob);
		ThrowIfFailed(hr);

		return shaderBlob;
	}

	ComPtr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* pDevice, ID3DBlob* pVSBlob)
	{
		ComPtr<ID3D11VertexShader> vtxShader{};

		HRESULT hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
			nullptr, &vtxShader);
		ThrowIfFailed(hr);

		return vtxShader;
	}

	ComPtr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* pDevice, ID3DBlob* pPSBlob)
	{
		ComPtr<ID3D11PixelShader> pixelShader{};

		HRESULT hr = pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
			nullptr, &pixelShader);
		ThrowIfFailed(hr);

		return pixelShader;
	}
}