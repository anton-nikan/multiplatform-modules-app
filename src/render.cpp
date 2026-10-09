export module render;

import std;
import native_storage;
export import application;

using namespace std;

export namespace render {
	struct context {
		// Public part
		array<double, 4> clear_color{ 0.0, 0.0, 1.0, 1.0 };

		// Native part: only implementations know the type stored here.
		native_storage<64> native;
	};

	void startup(context& ctx, application::context& appctx);
	void shutdown(context& ctx);
}
