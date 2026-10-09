export module resources;

import std;
import model;

using namespace std;

export namespace resources {
	using resource_t = variant<monostate, model::model_t>;

	expected<resource_t, string> load(filesystem::path filename) {
		string extension = filename.extension().string();
		ranges::transform(extension, extension.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });

		if (extension == ".glb") {
			return model::load_binary_model(filename).transform([](model::model_t&& m) { return resource_t{ std::move(m) }; });
		}
		else if (extension == ".gltf") {
			return model::load_ascii_model(filename).transform([](model::model_t&& m) { return resource_t{ std::move(m) }; });
		}

		return unexpected{ format("Unsupported resource type '{}': {}", extension, filename.string()) };
	}
}
