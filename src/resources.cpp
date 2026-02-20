module;

export module resources;

import std;
using namespace std;

export namespace resources {
	using resource_t = variant<monostate>;
	resource_t load(filesystem::path filename) {
		return {};
	}
}
