module;

#include <SDL3/SDL.h>

export module application.native;

export namespace application::native {
	struct data {
		SDL_Window* window = nullptr;
	};
}
