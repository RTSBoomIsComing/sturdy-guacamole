#pragma once
//#include "Scene.h"

#include "GLTFBufferView.h"
#include "GLTFMesh.h"
#include "GLTFNode.h"
#include "GLTFScene.h"

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;


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


		std::vector<GLTFBufferView> m_bufferViews{};
		std::vector<GLTFMesh> m_meshes{};
		std::vector<GLTFNode> m_nodes{};
		std::vector<GLTFScene> m_scenes{};
		std::vector<ComPtr<ID3D11ShaderResourceView>> m_images{};
		std::vector<ComPtr<ID3D11Texture2D>> m_d3dtexture{};

	private:
		bool LoadModel(tinygltf::Model& outModel, const std::filesystem::path& path);
	};
}