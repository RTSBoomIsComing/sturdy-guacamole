#pragma once
#include <vector>
#include <memory>

namespace tinygltf
{
	class Model;
	struct Mesh;
}

namespace sturdy_guacamole
{
	class Mesh
	{
	public:
		Mesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh);
		~Mesh() = default;

		// meshPrimitive never be shared
		std::vector<std::unique_ptr<class MeshPrimitive>> m_meshPrimitives{};
	};
}
