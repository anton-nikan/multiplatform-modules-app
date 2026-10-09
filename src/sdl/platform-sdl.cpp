module;

#include <SDL3/SDL.h>

module platform;

import std;

using namespace std;

namespace platform {
	struct native_data {
		bool initialized = false;
	};

	void startup(context& ctx, const vector<string_view>& args) {
		copy(begin(args), end(args), ostream_iterator<string_view>{ cout, "\n" });
		ctx.args.assign(begin(args), end(args));
		auto& n = ctx.native.emplace<native_data>();
		n.initialized = SDL_Init(SDL_INIT_VIDEO);
		if (!n.initialized) {
			cerr << "SDL_Init failed: " << SDL_GetError() << endl;
		}
	}

	void shutdown(context& ctx) {
		if (!ctx.native.has_value()) {
			return; // startup never ran
		}
		if (ctx.native.as<native_data>().initialized) {
			SDL_Quit();
		}
		ctx.native.reset();
	}
}
