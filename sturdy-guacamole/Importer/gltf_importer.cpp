#include "gltf_importer.h"

#include <windows.h>
#include <format>
#include <string>

sturdy_guacamole::importer::GLTFImporter::GLTFImporter(const std::filesystem::path& file_path)
{
	bool result = this->Load(file_path, this);
	if (!result) {
		throw std::exception{ "Failed to load glTF file. Check debug messages." };
	}

}

bool sturdy_guacamole::importer::GLTFImporter::Load(const std::filesystem::path& file_path, tinygltf::Model* tiny_model)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	std::string pathStr = file_path.string();
	bool is_success = loader.LoadASCIIFromFile(tiny_model, &err, &warn, pathStr);

	if (!warn.empty()) {
		const std::string warnMsg = std::format("WARN: {}", warn);
		OutputDebugStringA(warnMsg.c_str());
	}

	if (!err.empty()) {
		const std::string errMsg = std::format("ERR: {}", err);
		OutputDebugStringA(errMsg.c_str());
	}

	if (!is_success) {
		const std::string errMsg = std::format("Failed to load glTF: {}", pathStr);
		OutputDebugStringA(errMsg.c_str());
	}
	else {
		const std::string successMsg = std::format("Loaded glTF: {}", pathStr);
		OutputDebugStringA(successMsg.c_str());
	}

	return is_success;
}