#pragma once

#include <directxtk/SimpleMath.h>
namespace sturdy_guacamole
{
	// glTF scene node
	class SceneNode
	{
	public:
		SceneNode();
		~SceneNode() = default;

		// there is no reason to have TRS, only global transform is desired
		DirectX::SimpleMath::Matrix m_globalTransform{};
	};
}

