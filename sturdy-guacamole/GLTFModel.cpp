#include "GLTFModel.h"	

#include <tiny_gltf.h>

#include <iostream>
#include <cassert>

sturdy_guacamole::GLTFModel::GLTFModel(const std::filesystem::path& path)
{
	using namespace DirectX::SimpleMath;

	tinygltf::Model tinyModel;
	if (!LoadModel(tinyModel, path))
		throw std::exception("Failed to load glTF");

	m_meshes.reserve(tinyModel.meshes.size());
	for (const auto& tinyMesh : tinyModel.meshes)
	{
		m_meshes.push_back(Mesh{ tinyModel, tinyMesh });
	}

	m_nodes.reserve(tinyModel.nodes.size());
	for (const auto& tinyNode : tinyModel.nodes)
	{
		Node node{};
		node.m_name = tinyNode.name;
		if (static_cast<size_t>(tinyNode.mesh) < tinyModel.meshes.size())
			node.m_mesh = &m_meshes[tinyNode.mesh];

		for (int i : tinyNode.children)
		{
			node.m_children.push_back(&m_nodes[i]);
		}

		if (tinyNode.scale.size() > 0)
			node.m_scale = { (float)tinyNode.scale[0], (float)tinyNode.scale[1], (float)tinyNode.scale[2] };
		
		if (tinyNode.rotation.size() > 0)
			node.m_rotation = { (float)tinyNode.rotation[0], (float)tinyNode.rotation[1],(float)tinyNode.rotation[2], (float)tinyNode.rotation[3] };
		
		if (tinyNode.translation.size() > 0)
			node.m_translation = { (float)tinyNode.translation[0], (float)tinyNode.translation[1], (float)tinyNode.translation[2] };

		for (size_t i{}; i < tinyNode.matrix.size(); i++)
		{
			node.m_matrix.m[i / 4][i % 4] = static_cast<float>(tinyNode.matrix[i]);
		}

		if (tinyNode.matrix.empty())
			node.m_matrix = Matrix::CreateScale(node.m_scale)
			* Matrix::CreateFromQuaternion(node.m_rotation)
			* Matrix::CreateTranslation(node.m_translation);

		m_nodes.push_back(node);
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
