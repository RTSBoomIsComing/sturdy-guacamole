#include "Model.h"
#include "../Importer/gltf_importer.h"

sturdy_guacamole::rendering::Model::Model(const std::filesystem::path& file_name)
{
	importer::GLTFImporter gltf{ file_name };

	// Process nodes
	m_nodes.resize(gltf.nodes.size());
	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		m_nodes.get<Name>(i).str = gltf.nodes[i].name;
	}

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		const size_t children_count = gltf.nodes[i].children.size();
		m_nodes.get<Children>(i).count = static_cast<uint32_t>(children_count);


		if (gltf.nodes[i].children.empty()) {
			continue;
		}

		const int first_child = gltf.nodes[i].children.front();
		m_nodes.get<Children>(i).first = static_cast<uint16_t>(first_child);

		for (size_t child_id{}; child_id < children_count; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			m_nodes.get<Parent>(curr_sbling).id = static_cast<uint16_t>(i);
		}

		for (size_t child_id{}; child_id < children_count - 1; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			const int next_sbling = gltf.nodes[i].children[child_id + 1];
			m_nodes.get<Sibling>(curr_sbling).next = static_cast<uint16_t>(next_sbling);
		}

		for (size_t child_id{ 1 }; child_id < children_count; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			const int prev_sbling = gltf.nodes[i].children[child_id - 1];
			m_nodes.get<Sibling>(curr_sbling).prev = static_cast<uint16_t>(prev_sbling);
		}
	}

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		using namespace DirectX::SimpleMath;

		std::array<float, 16> floats{};

		if (!gltf.nodes[i].translation.empty())
		{
			for (size_t j{}; j < gltf.nodes[i].translation.size(); j++) {
				floats[j] = static_cast<float>(gltf.nodes[i].matrix[j]);
			}
			m_nodes.get<Translation>(i).vec3 = Vector3{ floats.data() };
		}

		if (!gltf.nodes[i].rotation.empty()) {
			for (size_t j{}; j < gltf.nodes[i].rotation.size(); j++) {
				floats[j] = static_cast<float>(gltf.nodes[i].matrix[j]);
			}
			m_nodes.get<Rotation>(i).quat = Quaternion{ floats.data() };
		}

		if (!gltf.nodes[i].scale.empty()) {
			for (size_t j{}; j < gltf.nodes[i].scale.size(); j++) {
				floats[j] = static_cast<float>(gltf.nodes[i].matrix[j]);
			}
			m_nodes.get<Scale>(i).vec3 = Vector3{ floats.data() };
		}

		if (!gltf.nodes[i].matrix.empty())
		{
			for (size_t j{}; j < gltf.nodes[i].matrix.size(); j++) {
				floats[j] = static_cast<float>(gltf.nodes[i].matrix[j]);
			}
			m_nodes.get<Transform>(i).matrix = Matrix{ floats.data() };
		}
		else
		{
			// NOTE : row major matrix
			Matrix SRT = Matrix::CreateScale(m_nodes.get<Scale>(i).vec3)
				* Matrix::CreateFromQuaternion(m_nodes.get<Rotation>(i).quat)
				* Matrix::CreateTranslation(m_nodes.get<Translation>(i).vec3);

			m_nodes.get<Transform>(i).matrix = SRT;
		}
	}

}
