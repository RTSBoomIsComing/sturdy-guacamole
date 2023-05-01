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
		ThrowIfFailed(::D3DReadFileToBlob(csoPath.c_str(), &shaderBlob));

		return shaderBlob;
	}
}