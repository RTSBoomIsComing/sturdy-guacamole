#include "GLTFDx11Helpers.h"
#include <tiny_gltf.h>

D3D_PRIMITIVE_TOPOLOGY sturdy_guacamole::ConvertToDx11Topology(int gltfPrimitiveMode)
{
	switch (gltfPrimitiveMode)
	{
	case TINYGLTF_MODE_TRIANGLES:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case TINYGLTF_MODE_TRIANGLE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case TINYGLTF_MODE_POINTS:
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case TINYGLTF_MODE_LINE:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case TINYGLTF_MODE_LINE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case TINYGLTF_MODE_TRIANGLE_FAN:
		OutputDebugStringW(L"TINYGLTF_MODE_TRIANGLE_FAN is not supported");
		break;
	case TINYGLTF_MODE_LINE_LOOP:
		OutputDebugStringW(L"TINYGLTF_MODE_LINE_LOOP is not supported");
		break;
	}

	return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

DXGI_FORMAT sturdy_guacamole::ConvertToDXGIFormat(int compType, int compCount, bool srgb)
{
	if (srgb && compType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE && compCount == 4)
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	}

	if (compType == TINYGLTF_COMPONENT_TYPE_FLOAT)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R32_FLOAT;
		case 2:
			return DXGI_FORMAT_R32G32_FLOAT;
		case 3:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case 4:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
	}
	else if (compType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R8_UNORM;
		case 2:
			return DXGI_FORMAT_R8G8_UNORM;
		case 4:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		}
	}
	else if (compType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R16_UINT;
		case 2:
			return DXGI_FORMAT_R16G16_UINT;
		case 4:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		}
	}
	else if (compType == TINYGLTF_COMPONENT_TYPE_SHORT)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R16_SINT;
		case 2:
			return DXGI_FORMAT_R16G16_SINT;
		case 4:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		}
	}
	else if (compType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R32_UINT;
		case 2:
			return DXGI_FORMAT_R32G32_UINT;
		case 3:
			return DXGI_FORMAT_R32G32B32_UINT;
		case 4:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		}
	}
	else if (compType == TINYGLTF_COMPONENT_TYPE_BYTE)
	{
		switch (compCount)
		{
		case 1:
			return DXGI_FORMAT_R8_SINT;
		case 2:
			return DXGI_FORMAT_R8G8_SINT;
		case 4:
			return DXGI_FORMAT_R8G8B8A8_SINT;
		}
	}

	return DXGI_FORMAT_UNKNOWN;
}

D3D11_SAMPLER_DESC sturdy_guacamole::ConvertToDx11SamplerDesc(int magFilter, int minFilter, int wrapS, int wrapT)
{
	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);


	if (magFilter == TINYGLTF_TEXTURE_FILTER_NEAREST)
	{
		switch (minFilter)
		{
		case TINYGLTF_TEXTURE_FILTER_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			break;
		}
	}

	if (magFilter == TINYGLTF_TEXTURE_FILTER_LINEAR)
	{
		switch (minFilter)
		{
		case TINYGLTF_TEXTURE_FILTER_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			break;
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			break;
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			break;
		}
	}

	switch (wrapS)
	{
	case TINYGLTF_TEXTURE_WRAP_REPEAT:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	}

	switch (wrapT)
	{
	case TINYGLTF_TEXTURE_WRAP_REPEAT:
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	}

	return samplerDesc;
}
