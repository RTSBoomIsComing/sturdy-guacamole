#include "Model.h"
#include "../Importer/gltf_importer.h"
#include "../Utility/FloatHelper.h"

sturdy_guacamole::rendering::Model::Model(const std::filesystem::path& file_name)
{
	importer::GLTFImporter gltf{ file_name };
	this->ProcessNodes(gltf);
	this->ProcessScenes(gltf);
}

void sturdy_guacamole::rendering::Model::ProcessNodes(importer::GLTFImporter const& gltf)
{
	m_nodes.resize(gltf.nodes.size());

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		m_nodes.get<Name>(i).str = gltf.nodes[i].name;
	}

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		const size_t children_count = gltf.nodes[i].children.size();
		m_nodes.get<Children>(i).count = static_cast<std::uint32_t>(children_count);


		if (gltf.nodes[i].children.empty()) {
			continue;
		}

		const int first_child = gltf.nodes[i].children.front();
		m_nodes.get<Children>(i).first = static_cast<std::uint16_t>(first_child);

		for (size_t child_id{}; child_id < children_count; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			m_nodes.get<Parent>(curr_sbling).id = static_cast<std::uint16_t>(i);
		}

		for (size_t child_id{}; child_id < children_count - 1; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			const int next_sbling = gltf.nodes[i].children[child_id + 1];
			m_nodes.get<Sibling>(curr_sbling).next = static_cast<std::uint16_t>(next_sbling);
		}

		for (size_t child_id{ 1 }; child_id < children_count; child_id++)
		{
			const int curr_sbling = gltf.nodes[i].children[child_id];
			const int prev_sbling = gltf.nodes[i].children[child_id - 1];
			m_nodes.get<Sibling>(curr_sbling).prev = static_cast<std::uint16_t>(prev_sbling);
		}
	}

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		using namespace DirectX::SimpleMath;

		if (!gltf.nodes[i].translation.empty())
		{
			const auto floats = utility::ConvertToFloatArray<const double, 3>(gltf.nodes[i].translation);
			m_nodes.get<Translation>(i).vec3 = Vector3{ floats.data() };
		}

		if (!gltf.nodes[i].rotation.empty()) {
			const auto floats = utility::ConvertToFloatArray<const double, 4>(gltf.nodes[i].rotation);
			m_nodes.get<Rotation>(i).quat = Quaternion{ floats.data() };
		}

		if (!gltf.nodes[i].scale.empty()) {
			const auto floats = utility::ConvertToFloatArray<const double, 3>(gltf.nodes[i].scale);
			m_nodes.get<Scale>(i).vec3 = Vector3{ floats.data() };
		}

		if (!gltf.nodes[i].matrix.empty())
		{
			const auto floats = utility::ConvertToFloatArray<const double, 16>(gltf.nodes[i].matrix);
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

	for (size_t i{}; i < gltf.nodes.size(); i++)
	{
		if (gltf.nodes[i].mesh < 0) {
			continue;
		}

		m_nodes.get<Mesh>(i).id = static_cast<std::uint16_t>(gltf.nodes[i].mesh);
	}
}

void sturdy_guacamole::rendering::Model::ProcessScenes(importer::GLTFImporter const& gltf)
{
	m_scenes.resize(gltf.scenes.size());
	for (size_t i{}; i < gltf.scenes.size(); i++)
	{
		m_scenes[i].name = gltf.scenes[i].name;

		for (const int rootNode : gltf.scenes[i].nodes)
		{
			m_scenes[i].roots.push_back(static_cast<std::uint16_t>(rootNode));
		}
	}
}
