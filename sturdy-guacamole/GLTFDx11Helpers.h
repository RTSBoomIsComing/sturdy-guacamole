#pragma once

#include <d3d11.h>

namespace sturdy_guacamole
{
	D3D_PRIMITIVE_TOPOLOGY ConvertToDx11Topology(int gltfPrimitiveMode);
	DXGI_FORMAT ConvertToDXGIFormat(int compType, int compCount, bool srgb = false);
}

