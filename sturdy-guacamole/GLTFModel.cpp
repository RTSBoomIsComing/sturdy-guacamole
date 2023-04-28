#include "GLTFModel.h"

#include <tiny_gltf.h>

#include <iostream>

sturdy_guacamole::GLTFModel::GLTFModel(const std::filesystem::path& path)
{
	tinygltf::Model tinyModel;
	bool res = LoadModel(tinyModel, path);
	if (!res)
	{
		std::cout << "Failed to load glTF: " << path.string() << std::endl;
		throw std::exception("Failed to load glTF");
	}

	for (const auto& tinyMesh : tinyModel.meshes)
	{
		m_meshes.push_back(Mesh{ tinyModel, tinyMesh });
	}

	for (const auto& scene : tinyModel.scenes)
	{
		m_scenes.push_back(Scene{ scene, tinyModel, m_meshes });
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
