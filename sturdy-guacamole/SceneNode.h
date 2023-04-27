#pragma once

#include <directxtk/SimpleMath.h>

#include <memory>

namespace sturdy_guacamole
{
	// forward declaration
	class Mesh;

	// glTF scene node
	class SceneNode
	{
	public:
		SceneNode();
		~SceneNode() = default;

		// there is no reason to have TRS, only global transform is desired
		DirectX::SimpleMath::Matrix m_globalTransform{};

		std::shared_ptr<Mesh> m_mesh{};
	};
}

