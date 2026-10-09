module;

#include <SDL3/SDL.h>

module render;

import std;
import application;
import application.native;

using namespace std;

namespace render {
	struct native_data {
		SDL_Renderer* renderer = nullptr;
		bool vsync = false;
	};

	void startup(context& ctx, application::context& appctx) {
		auto& n = ctx.native.emplace<native_data>();
		n.renderer = SDL_CreateRenderer(appctx.native.as<application::native::data>().window, nullptr);
		if (n.renderer == nullptr) {
			cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << endl;
			return;
		}
		n.vsync = SDL_SetRenderVSync(n.renderer, 1);
		if (!n.vsync) {
			cerr << "VSync is not available, limiting the frame rate: " << SDL_GetError() << endl;
		}

		// SDL has no view delegate that draws on its own, the application loop calls this.
		appctx.on_frame = [&ctx] {
			auto& n = ctx.native.as<native_data>();
			SDL_SetRenderDrawColorFloat(n.renderer,
				static_cast<float>(ctx.clear_color[0]), static_cast<float>(ctx.clear_color[1]),
				static_cast<float>(ctx.clear_color[2]), static_cast<float>(ctx.clear_color[3]));
			SDL_RenderClear(n.renderer);
			SDL_RenderPresent(n.renderer);
			if (!n.vsync) {
				SDL_Delay(16);
			}
		};
	}

	void shutdown(context& ctx) {
		if (!ctx.native.has_value()) {
			return; // startup never ran
		}
		auto& n = ctx.native.as<native_data>();
		if (n.renderer != nullptr) {
			SDL_DestroyRenderer(n.renderer);
		}
		ctx.native.reset();
	}
}
