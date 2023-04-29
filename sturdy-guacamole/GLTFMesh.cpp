#include "GLTFMesh.h"

#include <tiny_gltf.h>

sturdy_guacamole::GLTFMesh::GLTFMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
	: m_name{ mesh.name }
{
	for (const auto& primitive : mesh.primitives)
	{
		m_meshPrimitives.push_back(MeshPrimitive(model, primitive));
	}
}
