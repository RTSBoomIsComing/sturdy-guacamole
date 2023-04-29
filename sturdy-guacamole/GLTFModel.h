#pragma once
//#include "Scene.h"
#include "GLTFMesh.h"
#include "GLTFNode.h"
#include "GLTFScene.h"

#include <directxtk/SimpleMath.h>

#include <vector>
#include <filesystem>

namespace tinygltf
{
	class Model;
	class Node;
}

namespace sturdy_guacamole
{
	class GLTFModel
	{
	public:
		GLTFModel(const std::filesystem::path& path);
		~GLTFModel() = default;


		std::vector<GLTFNode> m_nodes{};
		std::vector<GLTFMesh> m_meshes{};
		std::vector<GLTFScene> m_scenes{};

	private:
		bool LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path);
	};
}