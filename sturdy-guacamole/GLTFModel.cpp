#include "GLTFModel.h"	

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

	// populate meshes
	m_meshes.reserve(tinyModel.meshes.size());
	for (const auto& tinyMesh : tinyModel.meshes)
	{
		m_meshes.push_back(Mesh{ tinyModel, tinyMesh });
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