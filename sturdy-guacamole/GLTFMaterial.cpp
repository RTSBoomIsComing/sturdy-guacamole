#include "GLTFMaterial.h"
#include "GLTFModel.h"

#include "Dx11Application.h"
#include "Dx11Helpers.h"
#include "Graphics.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFMaterial::GLTFMaterial(const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMat, const GLTFModel& myModel)
	: m_name{ tinyMat.name }
{
	ProcessTexture(tinyMat.pbrMetallicRoughness.baseColorTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.pbrMetallicRoughness.metallicRoughnessTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.normalTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.occlusionTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.emissiveTexture.index, tinyModel, myModel);

	for (size_t i{}; i < 4; ++i)
	{
		m_baseColorFactor[i] = static_cast<float>(tinyMat.pbrMetallicRoughness.baseColorFactor[i]);
	}

	m_metallicFactor = static_cast<float>(tinyMat.pbrMetallicRoughness.metallicFactor);
	m_roughnessFactor = static_cast<float>(tinyMat.pbrMetallicRoughness.roughnessFactor);

	for (size_t i{}; i < 3; ++i)
	{
		m_emissiveFactor[i] = static_cast<float>(tinyMat.emissiveFactor[i]);
	}

	// Create material constant buffer
	struct
	{
		float BaseColorFactor[4];
		float EmissiveFactor[3];
		float MetallicFactor;
		float RoughnessFactor;

		BOOL HasBaseColorTex;
		BOOL HasMetallicRoughnessTex;
		BOOL HasNormalTex;
		BOOL HasOcclusionTex;
		BOOL HasEmissiveTex;
		BOOL dummy[2];
	} materialConstants{};

	::memcpy(materialConstants.BaseColorFactor, m_baseColorFactor, sizeof(float) * 4);
	::memcpy(materialConstants.EmissiveFactor, m_emissiveFactor, sizeof(float) * 3);
	materialConstants.MetallicFactor = m_metallicFactor;
	materialConstants.RoughnessFactor = m_roughnessFactor;

	materialConstants.HasBaseColorTex = m_pSRViews[0] != nullptr;
	materialConstants.HasMetallicRoughnessTex = m_pSRViews[1] != nullptr;
	materialConstants.HasNormalTex = m_pSRViews[2] != nullptr;
	materialConstants.HasOcclusionTex = m_pSRViews[3] != nullptr;
	materialConstants.HasEmissiveTex = m_pSRViews[4] != nullptr;

	CD3D11_BUFFER_DESC bufferDesc{ sizeof(materialConstants), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE };
	D3D11_SUBRESOURCE_DATA initData{ &materialConstants };
	ThrowIfFailed(Dx11App::Get().m_device->CreateBuffer(&bufferDesc, &initData, &m_cbuffer_material));
}

void sturdy_guacamole::GLTFMaterial::ProcessTexture(size_t textureIdx, const tinygltf::Model& tinyModel, const GLTFModel& myModel)
{
	if (textureIdx < tinyModel.textures.size())
	{
		const auto& tinyTex = tinyModel.textures[textureIdx];
		m_pSRViews.push_back(myModel.m_images[tinyTex.source].Get());
		m_pSamplers.push_back(myModel.m_samplers[tinyTex.sampler].Get());
	}
	else
	{
		m_pSRViews.push_back(nullptr);

		// if set nullptr to sampler, graphics debugger will show warning message, 
		// so set our linear sampler for default
		m_pSamplers.push_back(Graphics::Get().m_sampler.linear.Get()); 
	}

}
