#pragma once
#include "MeshPrimitive.h"

#include <string>
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

		std::string m_name{};

		std::vector<MeshPrimitive> m_meshPrimitives{};
	};
}
