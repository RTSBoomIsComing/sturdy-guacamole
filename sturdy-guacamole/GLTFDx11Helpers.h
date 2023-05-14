#pragma once

#include <d3d11.h>

namespace sturdy_guacamole
{
	D3D_PRIMITIVE_TOPOLOGY ConvertToDx11Topology(int gltfPrimitiveMode);
	DXGI_FORMAT ConvertToDXGIFormat(int compType, int compCount, bool srgb = false, bool unorm = false);
	D3D11_SAMPLER_DESC ConvertToDx11SamplerDesc(int magFilter, int minFilter, int wrapS, int wrapT);
	unsigned int GetNumberOfComponents(int accessorType);
}

