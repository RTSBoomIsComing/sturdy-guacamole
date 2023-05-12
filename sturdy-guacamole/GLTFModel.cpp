#include "GLTFModel.h"	
#include "GLTFDx11Helpers.h"
#include "Dx11Application.h"
#include "Dx11Helpers.h"
// include tiny_gltf
#pragma warning( disable : 4996 )
#undef min
#undef max
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>
#include <iostream>
#include <cassert>

sturdy_guacamole::GLTFModel::GLTFModel(const std::filesystem::path& path)
{
	using namespace DirectX::SimpleMath;

	tinygltf::Model tinyModel;
	if (!LoadModel(tinyModel, path))
		throw std::exception("Failed to load glTF");

	// populate buffer views
	m_bufferViews.reserve(tinyModel.bufferViews.size());
	for (const auto& tinyBufferView : tinyModel.bufferViews)
	{
		m_bufferViews.push_back(GLTFBufferView{ tinyModel, tinyBufferView, *this });
	}

	// populate images
	m_images.reserve(tinyModel.images.size());
	for (const auto& tinyImage : tinyModel.images)
	{
		DXGI_FORMAT format = sturdy_guacamole::ConvertToDXGIFormat(tinyImage.pixel_type, tinyImage.component);

		ComPtr<ID3D11Texture2D> tex2d{};
		ComPtr<ID3D11ShaderResourceView> srv{};
		CD3D11_TEXTURE2D_DESC desc{ format, (UINT)tinyImage.width, (UINT)tinyImage.height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE };
		D3D11_SUBRESOURCE_DATA initData{};
		const void* pSysMem = const_cast<unsigned char*>(tinyImage.image.data());
		initData.pSysMem = tinyImage.image.data();
		initData.SysMemPitch = tinyImage.width * (tinyImage.bits / 8) * tinyImage.component;

		ThrowIfFailed(g_pDevice->CreateTexture2D(&desc, &initData, &tex2d));
		ThrowIfFailed(g_pDevice->CreateShaderResourceView(tex2d.Get(), nullptr, &srv));

		m_images.push_back(srv);
	}

	// populate samplers
	m_samplers.reserve(tinyModel.samplers.size());
	for (const auto& tinySampler : tinyModel.samplers)
	{
		auto samplerDesc = sturdy_guacamole::ConvertToDx11SamplerDesc(tinySampler.magFilter, tinySampler.minFilter, tinySampler.wrapS, tinySampler.wrapT);

		ComPtr<ID3D11SamplerState> sampler{};
		ThrowIfFailed(g_pDevice->CreateSamplerState(&samplerDesc, &sampler));

		m_samplers.push_back(sampler);
	}

	// populate materials
	m_materials.reserve(tinyModel.materials.size());
	for (const auto& tinyMaterial : tinyModel.materials)
	{
		m_materials.push_back(GLTFMaterial{ tinyModel, tinyMaterial, *this });
	}

	// populate meshes
	m_meshes.reserve(tinyModel.meshes.size());
	for (const auto& tinyMesh : tinyModel.meshes)
	{
		m_meshes.push_back(GLTFMesh{ tinyModel, tinyMesh, *this });
	}

	// populate nodes, populate meshes has to be done first
	// don't use reserve, because we need to be able to use pointers to the nodes
	m_nodes.resize(tinyModel.nodes.size());
	for (size_t i{}; i < tinyModel.nodes.size(); i++)
	{
		m_nodes[i] = GLTFNode{ tinyModel.nodes[i], *this };
	}

	// populate scenes
	for (const auto& tinyScene : tinyModel.scenes)
	{
		m_scenes.push_back(GLTFScene{ tinyScene, *this });
	}

	// populate animations
	//for (const auto& tinyAnimation : tinyModel.animations)
	//{
	//	m_animations.push_back(GLTFAnimation{ tinyModel, tinyAnimation, *this });
	//}


}

bool sturdy_guacamole::GLTFModel::LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string pathStr = path.string();
	bool res = loader.LoadASCIIFromFile(&outModel, &err, &warn, pathStr);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << pathStr << std::endl;
	else
		std::cout << "Loaded glTF: " << pathStr << std::endl;

	return res;
}