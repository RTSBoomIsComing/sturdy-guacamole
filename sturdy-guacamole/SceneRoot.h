#pragma once
#include "SceneNode.h"

namespace sturdy_guacamole
{
	class SceneRoot
	{
	public:
		SceneRoot(const tinygltf::Node& node, const tinygltf::Model& model);
		~SceneRoot() = default;
		std::vector<std::shared_ptr<SceneNode>> m_dfsList;

	private:
		void DFS(const tinygltf::Node& node, const tinygltf::Model& model, std::shared_ptr<SceneNode> parent);
	};
}


