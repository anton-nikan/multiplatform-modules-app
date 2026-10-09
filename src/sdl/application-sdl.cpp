module;

#include <SDL3/SDL.h>

module application;

import std;
import application.native;

using namespace std;

namespace application {
	void startup(context& ctx) {
		ctx.native.emplace<native::data>();
	}

	void run(context& ctx) {
		auto& n = ctx.native.as<native::data>();
		n.window = SDL_CreateWindow("Game", static_cast<int>(ctx.frame.width), static_cast<int>(ctx.frame.height), 0);
		if (n.window == nullptr) {
			cerr << "SDL_CreateWindow failed: " << SDL_GetError() << endl;
			return;
		}

		if (ctx.did_finish_launching) {
			ctx.did_finish_launching();
		}

		for (bool running = true; running;) {
			for (SDL_Event event; SDL_PollEvent(&event);) {
				if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
					running = false;
				}
			}
			if (ctx.on_frame) {
				ctx.on_frame();
			}
			else {
				SDL_Delay(10);
			}
		}
	}

	void shutdown(context& ctx) {
		if (!ctx.native.has_value()) {
			return; // startup never ran
		}
		auto& n = ctx.native.as<native::data>();
		if (n.window != nullptr) {
			SDL_DestroyWindow(n.window);
			n.window = nullptr;
		}
		ctx.native.reset();
	}
}
