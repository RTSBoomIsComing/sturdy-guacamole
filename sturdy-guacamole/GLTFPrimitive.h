#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <vector>
#include <string>

namespace tinygltf
{
	class Model;
	struct Primitive;
}

namespace sturdy_guacamole
{
	class GLTFModel;

	// this class deal with the mesh primitive of glTF
	class GLTFPrimitive
	{
	public:
		GLTFPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		~GLTFPrimitive() = default;
		void Draw(ID3D11DeviceContext* pDeviceContext) const;
		void DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount);


	public:
		// used in ID3D11DeviceContext::IASetIndexBuffer();
		ID3D11Buffer* m_indexBuffer{};
		DXGI_FORMAT m_indexBufferFormat = DXGI_FORMAT_R16_UINT; // or DXGI_FORMAT_R32_UINT

		// it would be always 0. once creating index buffer, the offset was already applied
		UINT m_indexBufferOffset{};

		// used in ID3D11DeviceContext::DrawIndexed();
		UINT m_indexCount{};
	
		// used in ID3D11DeviceContext::IASetVertexBuffers();
		std::vector<ID3D11Buffer*> m_vertexBuffers{};
		std::vector<UINT> m_vertexBufferStrides{};
		std::vector<UINT> m_vertexBufferOffsets{};

		std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputElementDescs{};

		// used in ID3D11DeviceContext::IASetPrimitiveTopology();
		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	private:
		void ProcessIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		void ProcessAttributes(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		bool SetPrimitiveTopology(const tinygltf::Primitive& primitive);
	};
}

