#pragma once

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <string>

namespace tinygltf
{
	class Model;
	struct Material;
}

namespace sturdy_guacamole
{
	class GLTFModel;

	class GLTFMaterial
	{
	public:
		GLTFMaterial(const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMat, const GLTFModel& myModel);
		~GLTFMaterial() = default;
		std::string m_name{};

		// [0] = baseColorTexture, [1] = metallicRoughnessTexture, [2] = normalTexture, [3] = occlusionTexture, [4] = emissiveTexture
		std::vector<ID3D11ShaderResourceView*> m_pSRViews{};
		std::vector<ID3D11SamplerState*> m_pSamplers{};

		float m_baseColorFactor[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
		float m_metallicFactor{ 1.0f };
		float m_roughnessFactor{ 1.0f };
		float m_emissiveFactor[3]{};


	private:
		void ProcessTexture(size_t textureIdx, const tinygltf::Model& tinyModel, const GLTFModel& myModel);
	};
}

