#pragma once
#include <directxtk/SimpleMath.h>

namespace sturdy_guacamole
{
	struct MeshConstants
	{
		DirectX::SimpleMath::Matrix WorldMatrix{};	// Model to world
		DirectX::SimpleMath::Matrix WorldIT{};		// Model normals to world
	};

	struct CommonConstants
	{
		DirectX::SimpleMath::Matrix ViewMatrix{};		// World to view
		DirectX::SimpleMath::Matrix ViewInv{};			// View to world
		DirectX::SimpleMath::Matrix ProjMatrix{};		// View to NDC
		DirectX::SimpleMath::Matrix ProjInv{};			// NDC to view
		DirectX::SimpleMath::Matrix ViewProjMatrix{};	// World to NDC
		DirectX::SimpleMath::Matrix ViewProjInv{};		// NDC to world

		DirectX::SimpleMath::Vector4 ViewerPos{};		// World space viewer position
	};
}
