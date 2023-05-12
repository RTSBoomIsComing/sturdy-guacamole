#pragma once
#include <string>
#include <vector>
#include <directxtk/SimpleMath.h>
namespace tinygltf
{
	class Model;
	struct Animation;
}

namespace sturdy_guacamole
{
	class GLTFModel;
	class GLTFMesh;

	class GLTFAnimation
	{
	public:
		GLTFAnimation(const tinygltf::Model& tinyModel, const tinygltf::Animation& tinyAnim, const sturdy_guacamole::GLTFModel& myModel);
		~GLTFAnimation() = default;

		std::string m_name{};

		struct Channel
		{
			int m_nodeIndex{};
			std::vector<float> m_times{};
			DirectX::SimpleMath::Vector3 translation{};
			DirectX::SimpleMath::Quaternion rotation{};
			DirectX::SimpleMath::Vector3 scale{ 1.0f, 1.0f, 1.0f };
		};

		std::vector<Channel> m_channels{};


	};
}