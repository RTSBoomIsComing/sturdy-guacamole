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
	class GLTFMesh
	{
	public:
		GLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh);
		~GLTFMesh() = default;

		std::string m_name{};

		std::vector<MeshPrimitive> m_meshPrimitives{};
	};
}
