#include "Mesh.h"
#include "MeshPrimitive.h"

#include <tiny_gltf.h>

sturdy_guacamole::Mesh::Mesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh)
{
	for (const auto& primitive : mesh.primitives)
	{
		m_meshPrimitives.push_back(std::make_unique<MeshPrimitive>(model, primitive));
	}
}
