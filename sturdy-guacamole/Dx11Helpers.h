#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <filesystem>

namespace sturdy_guacamole
{
	inline void ThrowIfFailed(HRESULT hr);
	
	ComPtr<ID3DBlob> CreateShaderBlob(std::filesystem::path csoPath);
}