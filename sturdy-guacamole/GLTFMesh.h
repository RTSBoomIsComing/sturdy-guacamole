#pragma once
#include "GLTFPrimitive.h"

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
	class GLTFModel;
	class GLTFMesh
	{
	public:
		GLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const GLTFModel& myModel);
		~GLTFMesh() = default;

		std::string m_name{};

		std::vector<GLTFPrimitive> m_meshPrimitives{};
	};
}
