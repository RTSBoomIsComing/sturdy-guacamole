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
	class GLTFBufferView;

	class GLTFPrimitive
	{
	public:
		GLTFPrimitive(const tinygltf::Model& tinyModel, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		~GLTFPrimitive() = default;
		void Draw(ID3D11DeviceContext* pDeviceContext) const;
		void DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount);


	public:
		
		struct // used in ID3D11DeviceContext::IASetIndexBuffer();
		{
			ID3D11Buffer* pBuffer{};
			DXGI_FORMAT format = DXGI_FORMAT_R16_UINT; // or DXGI_FORMAT_R32_UINT
			UINT offset{};	// would be always 0

			// used in ID3D11DeviceContext::DrawIndexed();
			UINT count{};	
		} m_index;
	
		
		struct // used in ID3D11DeviceContext::IASetVertexBuffers();
		{
			std::vector<ID3D11Buffer*> pBuffers{};
			std::vector<UINT> strides{};
			std::vector<UINT> offsets{};
		} m_vertex;

		// used in ID3D11Device::IASetInputLayout();
		ComPtr<ID3D11InputLayout> m_inputLayout{};

		// used in ID3D11DeviceContext::IASetPrimitiveTopology();
		D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	private:
		void ProcessIndices(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		void ProcessAttributes(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel);
		//bool SetPrimitiveTopology(const tinygltf::Primitive& primitive);
		void AddVertexBuffer(ID3D11Buffer* pBuffer, UINT stride, UINT offset);
		UINT FindVertexBuffer(const ID3D11Buffer* pVertexBuffer) const;
	};
}

