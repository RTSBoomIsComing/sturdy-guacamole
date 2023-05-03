#include "GLTFMaterial.h"
#include "GLTFModel.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFMaterial::GLTFMaterial(const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMat, const GLTFModel& myModel)
	: m_name{ tinyMat.name }
{
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

	ProcessTexture(tinyMat.pbrMetallicRoughness.baseColorTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.pbrMetallicRoughness.metallicRoughnessTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.normalTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.occlusionTexture.index, tinyModel, myModel);
	ProcessTexture(tinyMat.emissiveTexture.index, tinyModel, myModel);
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
		m_pSamplers.push_back(nullptr);
	}

}
