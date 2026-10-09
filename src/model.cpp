export module model;

import std;

export namespace model {
	struct model_t {
		model_t() = default;
		model_t(model_t&&);
		model_t& operator= (model_t&&);

		~model_t();

		struct native_model_t;
		std::unique_ptr<native_model_t> model;
	};

	std::expected<model_t, std::string> load_ascii_model(std::filesystem::path filename);
	std::expected<model_t, std::string> load_binary_model(std::filesystem::path filename);
}
