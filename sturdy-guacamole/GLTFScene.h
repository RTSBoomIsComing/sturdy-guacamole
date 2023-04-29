#pragma once

#include <directxtk/SimpleMath.h>

#include <string>
#include <vector>

namespace tinygltf
{
	class Model;
	struct Scene;
}

namespace sturdy_guacamole
{
	class GLTFModel;
	class GLTFNode;

	class GLTFScene
	{
	public:
		struct traversalStep
		{
			const GLTFNode* m_node{};
			const GLTFNode* m_parent{}; // nullptr if root
			DirectX::SimpleMath::Matrix m_globalTransform{};
		};
		using Traversal = std::vector<traversalStep>;
		GLTFScene(const tinygltf::Scene& tinyScene, const GLTFModel& myModel);
		~GLTFScene() = default;
		std::string m_name{};
		std::vector<Traversal> m_traversals{};

		private:
			void DFS(const GLTFNode* node, const traversalStep* parent, Traversal& traversal);
	};
}


