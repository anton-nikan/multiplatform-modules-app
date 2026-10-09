module;

// Declarations only, the implementation is compiled in tinygltf.cpp.
#include "gltf.h"

module model;

import std;
using namespace std;

namespace model {
	struct model_t::native_model_t {
		tinygltf::Model m;
	};

	model_t::~model_t() = default;
	model_t::model_t(model_t&& v) = default;
	model_t& model_t::operator= (model_t&&) = default;

	template<typename Load>
	expected<model_t, string> load_model(const filesystem::path& filename, Load&& load) {
		tinygltf::TinyGLTF loader;
		string err;
		string warn;

		model_t ret{};
		ret.model = make_unique<model_t::native_model_t>();
		const bool res = load(loader, &ret.model->m, &err, &warn, filename.string());

		if (!warn.empty()) {
			clog << "WARN: " << warn << endl;
		}
		if (!res) {
			return unexpected{ format("Failed to load glTF {}: {}", filename.string(), err) };
		}
		return ret;
	}

	expected<model_t, string> load_ascii_model(filesystem::path filename) {
		return load_model(filename, [](auto& loader, auto* model, auto* err, auto* warn, const string& file) {
			return loader.LoadASCIIFromFile(model, err, warn, file);
		});
	}

	expected<model_t, string> load_binary_model(filesystem::path filename) {
		return load_model(filename, [](auto& loader, auto* model, auto* err, auto* warn, const string& file) {
			return loader.LoadBinaryFromFile(model, err, warn, file);
		});
	}
}
