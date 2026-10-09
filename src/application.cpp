export module application;

import std;
import native_storage;

using namespace std;

export namespace application {
	struct rect {
		double x, y, width, height;
	};

	struct context {
		// Public part
		rect frame{ 100.0, 100.0, 1024.0, 768.0 };
		function<void()> did_finish_launching;
		// Called once per iteration by backends that drive their own loop (the native macOS one does not need it).
		function<void()> on_frame;

		// Native part: only implementations know the type stored here.
		native_storage<64> native;
	};

	void startup(context& ctx);
	void run(context& ctx);
	void shutdown(context& ctx);
}
