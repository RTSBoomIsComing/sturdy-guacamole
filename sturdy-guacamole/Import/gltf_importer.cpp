#include "gltf_importer.h"

#include <format>
#include <string>

bool sturdy_guacamole::import::GLTFImporter::Import(const std::filesystem::path& file_name, rendering::Model& model)
{
	pModel = &model;
	tinygltf::Model tiny_model;
	bool result = this->Load(file_name, tiny_model);
	if (!result) {
		return false;
	}

	this->ProcessScenes(tiny_model.scenes);
	this->ProcessNodes(tiny_model.nodes);
	return true;
}

bool sturdy_guacamole::import::GLTFImporter::Load(const std::filesystem::path& file_name, tinygltf::Model& tiny_model)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string pathStr = file_name.string();
	bool result = loader.LoadASCIIFromFile(&tiny_model, &err, &warn, pathStr);

	if (!warn.empty()) {
		const std::string warnMsg = std::format("WARN: {}", warn);
		OutputDebugStringA(warnMsg.c_str());
	}

	if (!err.empty()) {
		const std::string errMsg = std::format("ERR: {}", err);
		OutputDebugStringA(errMsg.c_str());
	}

	if (!result) {
		const std::string errMsg = std::format("Failed to load glTF: {}", pathStr);
		OutputDebugStringA(errMsg.c_str());
	}
	else {
		const std::string successMsg = std::format("Loaded glTF: {}", pathStr);
		OutputDebugStringA(successMsg.c_str());
	}

	return result;
}

void sturdy_guacamole::import::GLTFImporter::ProcessScenes(const std::vector<tinygltf::Scene>& tiny_scenes)
{
	pModel->m_scene_list.name.reserve(tiny_scenes.size());
	for (const auto& tiny_scene : tiny_scenes) {
		pModel->m_scene_list.name.push_back(tiny_scene.name);
	}

	pModel->m_scene_list.first_scene_root.resize(tiny_scenes.size());
	pModel->m_scene_list.scene_root_count.resize(tiny_scenes.size());
	for (size_t scene_id{}; scene_id < tiny_scenes.size(); scene_id++)
	{
		const auto& tiny_scene = tiny_scenes[scene_id];
		pModel->m_scene_list.scene_root_count[scene_id] = static_cast<uint16_t>(tiny_scene.nodes.size());
		pModel->m_scene_list.first_scene_root[scene_id] = static_cast<uint16_t>(pModel->m_scene_root_list.size());

		pModel->m_scene_root_list.reserve(tiny_scene.nodes.size());
		bool is_first_scene_node{ true };
		for (const int node_id : tiny_scene.nodes)
		{
			sturdy_guacamole::rendering::Model::SceneRoot sceneRoot{};
			sceneRoot.root_node = node_id;

			if (!is_first_scene_node) {
				pModel->m_scene_root_list.back().next_sibling = static_cast<uint16_t>(pModel->m_scene_root_list.size());
			}

			pModel->m_scene_root_list.push_back(sceneRoot);
			is_first_scene_node = false;
		}
	}
}

void sturdy_guacamole::import::GLTFImporter::ProcessNodes(const std::vector<tinygltf::Node>& tiny_nodes)
{
	pModel->m_node_list.name.reserve(tiny_nodes.size());
	for (const auto& tiny_node : tiny_nodes) {
		pModel->m_node_list.name.push_back(tiny_node.name);
	}

	pModel->m_node_list.child_count.resize(tiny_nodes.size());
	pModel->m_node_list.first_child.resize(tiny_nodes.size());
	pModel->m_node_list.next_sibling.resize(tiny_nodes.size());
	for (size_t node_id{}; node_id < tiny_nodes.size(); node_id++)
	{
		const auto& tiny_node = tiny_nodes[node_id];
		pModel->m_node_list.child_count[node_id] = static_cast<uint16_t>(tiny_node.children.size());

		auto& first_child = pModel->m_node_list.first_child[node_id];

		first_child = static_cast<uint16_t>(tiny_node.children.front());
		pModel->m_node_list.parent[*first_child] = static_cast<uint16_t>(node_id);

		for (size_t i{ 1 }; i < tiny_node.children.size(); i++)
		{
			pModel->m_node_list.next_sibling[tiny_node.children[i - 1]] = static_cast<uint16_t>(tiny_node.children[i]);
		}
	}

	pModel->m_node_list.matrix.resize(tiny_nodes.size());
	pModel->m_node_list.translation.resize(tiny_nodes.size());
	pModel->m_node_list.rotation.resize(tiny_nodes.size());
	pModel->m_node_list.scale.resize(tiny_nodes.size());
	for (size_t node_id{}; node_id < tiny_nodes.size(); node_id++)
	{
		using namespace DirectX::SimpleMath;
		const auto& tiny_node = tiny_nodes[node_id];

		if (!tiny_node.translation.empty()) {
			const auto convert_translation = this->ConvertToFloatArray<const double, 3>(tiny_node.translation);
			pModel->m_node_list.translation[node_id] = Vector3{ convert_translation.data() };
		}

		if (!tiny_node.rotation.empty()) {
			const auto convert_rotation = this->ConvertToFloatArray<const double, 4>(tiny_node.rotation);
			pModel->m_node_list.rotation[node_id] = Quaternion{ convert_rotation.data() };
		}

		if (!tiny_node.scale.empty()) {
			const auto convert_scale = this->ConvertToFloatArray<const double, 3>(tiny_node.scale);
			pModel->m_node_list.scale[node_id] = Vector3{ convert_scale.data() };
		}

		if (!tiny_node.matrix.empty()) {
			const auto convert_matrix = this->ConvertToFloatArray<const double, 16>(tiny_node.matrix);
			pModel->m_node_list.matrix[node_id] = Matrix{ convert_matrix.data() };
		}
		else {
			// NOTE : row major matrix
			Matrix SRT = Matrix::CreateScale(pModel->m_node_list.scale[node_id]) *
				Matrix::CreateFromQuaternion(pModel->m_node_list.rotation[node_id]) *
				Matrix::CreateTranslation(pModel->m_node_list.translation[node_id]);
		}

	}

	pModel->m_node_list.mesh.resize(tiny_nodes.size());
	for (size_t node_id{}; node_id < tiny_nodes.size(); node_id++)
	{
		const auto& tiny_node = tiny_nodes[node_id];
		if (tiny_node.mesh >= 0) {
			pModel->m_node_list.mesh[node_id] = static_cast<uint16_t>(tiny_node.mesh);
		}
	}
}
