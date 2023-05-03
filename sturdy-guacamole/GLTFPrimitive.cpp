#include "GLTFPrimitive.h"
#include "GLTFModel.h"
#include "GLTFDx11Helpers.h"

#include "Dx11Application.h"
#include "Dx11Helpers.h"
#include "Graphics.h"

#include <tiny_gltf.h>

#include <iostream>

sturdy_guacamole::GLTFPrimitive::GLTFPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	m_topology = sturdy_guacamole::ConvertToDx11Topology(primitive.mode);
	ProcessIndices(model, primitive, myModel);
	ProcessAttributes(model, primitive, myModel);

	m_pMaterial = &myModel.m_materials[primitive.material];
}

void sturdy_guacamole::GLTFPrimitive::Draw(ID3D11DeviceContext* pDeviceContext) const
{
	pDeviceContext->IASetInputLayout(m_inputLayout.Get());
	pDeviceContext->IASetIndexBuffer(m_index.pBuffer, m_index.format, m_index.offset);
	pDeviceContext->IASetVertexBuffers(0, (UINT)m_vertex.pBuffers.size(), m_vertex.pBuffers.data(), m_vertex.strides.data(), m_vertex.offsets.data());
	pDeviceContext->DrawIndexed(m_index.count, 0, 0);
}

void sturdy_guacamole::GLTFPrimitive::DrawInstanced(ID3D11DeviceContext* pDeviceContext, UINT instanceCount)
{
	pDeviceContext->IASetInputLayout(m_inputLayout.Get());
	pDeviceContext->IASetIndexBuffer(m_index.pBuffer, m_index.format, m_index.offset);
	pDeviceContext->IASetVertexBuffers(0, (UINT)m_vertex.pBuffers.size(), m_vertex.pBuffers.data(), m_vertex.strides.data(), m_vertex.offsets.data());
	pDeviceContext->DrawIndexedInstanced(m_index.count, instanceCount, 0, 0, 0);
}

void sturdy_guacamole::GLTFPrimitive::ProcessIndices(const tinygltf::Model& tinyModel, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	const auto& accessor = tinyModel.accessors[primitive.indices];
	const int bufferViewIdx = accessor.bufferView;

	m_index.pBuffer = myModel.m_bufferViews[bufferViewIdx].m_buffer.Get();
	m_index.format = (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) ?
		DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	m_index.offset = 0;

	m_index.count = (UINT)accessor.count;
}

void sturdy_guacamole::GLTFPrimitive::ProcessAttributes(const tinygltf::Model& tinyModel, const tinygltf::Primitive& primitive, const GLTFModel& myModel)
{
	// used in ID3D11Device::CreateInputLayout();
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs{};

	const auto& attributes = primitive.attributes;

	{
		const int accessor_idx = attributes.at("POSITION");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const int byteStride = accessor.ByteStride(tinyModel.bufferViews[accessor.bufferView]);
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertex.pBuffers.size())
			this->AddVertexBuffer(myBufView.m_buffer.Get(), byteStride, 0);
				
		D3D11_INPUT_ELEMENT_DESC ieDesc{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, inputSlot, (UINT)accessor.byteOffset };
		inputElementDescs.push_back(ieDesc);
	}

	if (attributes.contains("NORMAL"))
	{
		const int accessor_idx = attributes.at("NORMAL");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const int byteStride = accessor.ByteStride(tinyModel.bufferViews[accessor.bufferView]);
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertex.pBuffers.size())
			this->AddVertexBuffer(myBufView.m_buffer.Get(), byteStride, 0);

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, inputSlot, (UINT)accessor.byteOffset };
		inputElementDescs.push_back(ieDesc);
	}
	else
	{
		const UINT inputSlot = this->FindVertexBuffer(nullptr);
		if (inputSlot == m_vertex.pBuffers.size())
			this->AddVertexBuffer(nullptr, 0, 0);

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, inputSlot };
		inputElementDescs.push_back(ieDesc);
	}

	if (attributes.contains("TEXCOORD_0"))
	{
		const int accessor_idx = attributes.at("TEXCOORD_0");
		const auto& accessor = tinyModel.accessors[accessor_idx];
		const int byteStride = accessor.ByteStride(tinyModel.bufferViews[accessor.bufferView]);
		const auto& myBufView = myModel.m_bufferViews[accessor.bufferView];

		UINT byteLength{};
		DXGI_FORMAT format = sturdy_guacamole::ConvertToDXGIFormat(accessor.componentType, 2);

		const UINT inputSlot = this->FindVertexBuffer(myBufView.m_buffer.Get());
		if (inputSlot == m_vertex.pBuffers.size())
			this->AddVertexBuffer(myBufView.m_buffer.Get(), byteStride, 0);

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "TEXCOORD", 0, format, inputSlot, (UINT)accessor.byteOffset };
		inputElementDescs.push_back(ieDesc);
	}
	else
	{
		const UINT inputSlot = this->FindVertexBuffer(nullptr);
		if (inputSlot == m_vertex.pBuffers.size())
			this->AddVertexBuffer(nullptr, 0, 0);

		D3D11_INPUT_ELEMENT_DESC ieDesc{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, inputSlot };
		inputElementDescs.push_back(ieDesc);
	}

	// create input layout
	const auto& vsBlob = Graphics::Get().m_vtxShader.basic_blob;
	ThrowIfFailed(g_pDevice->CreateInputLayout(inputElementDescs.data(), (UINT)inputElementDescs.size(),
		vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout));
}

void sturdy_guacamole::GLTFPrimitive::AddVertexBuffer(ID3D11Buffer* pBuffer, UINT stride, UINT offset)
{
	m_vertex.pBuffers.push_back(pBuffer);
	m_vertex.offsets.push_back(offset);
	m_vertex.strides.push_back(stride);
}

UINT sturdy_guacamole::GLTFPrimitive::FindVertexBuffer(const ID3D11Buffer* pVertexBuffer) const
{
	UINT inputSlot{};
	for (; inputSlot < m_vertex.pBuffers.size(); ++inputSlot)
	{
		if (m_vertex.pBuffers[inputSlot] == pVertexBuffer)
			break;
	}

	return inputSlot;
}
