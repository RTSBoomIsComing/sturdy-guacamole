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

	PopulateNodes(tinyModel);

	// populate scenes
	for (const auto& tinyScene : tinyModel.scenes)
	{
		Scene scene{};
		scene.m_name = tinyScene.name;

		
		scene.m_roots.reserve(tinyScene.nodes.size());
		for (int rootIdx : tinyScene.nodes)
		{
			scene.m_roots.push_back(&m_nodes[rootIdx]);
		}

		scene.m_dfsLists.resize(tinyScene.nodes.size());
		for (size_t i{}; i < scene.m_roots.size(); i++)
		{
			GenerateDFSList(scene.m_roots[i], nullptr, scene.m_dfsLists[i]);
		}



		m_scenes.push_back(scene);
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

void sturdy_guacamole::GLTFModel::PopulateNodes(const tinygltf::Model& tinyModel)
{
	using namespace DirectX::SimpleMath;
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
		{
			auto S = Matrix::CreateScale(node.m_scale);
			auto R = Matrix::CreateFromQuaternion(node.m_rotation);
			auto T = Matrix::CreateTranslation(node.m_translation);
			node.m_matrix = S * R * T;
		}

		m_nodes.push_back(node);
	}
}

void sturdy_guacamole::GLTFModel::GenerateDFSList(const Node* node, const DFSNode* parent, DFSList& dfsList) const
{
	using namespace DirectX::SimpleMath;
	DFSNode dfsNode{};
	dfsNode.m_node = node;
	dfsNode.m_parent = (parent) ? parent->m_node : nullptr;
	dfsNode.m_globalTransform = (parent) ? parent->m_globalTransform : Matrix::Identity;
	dfsNode.m_globalTransform *= node->m_matrix;

	dfsList.push_back(dfsNode);

	for (const Node* child : node->m_children)
	{
		GenerateDFSList(child, &dfsNode, dfsList);

	}
}
