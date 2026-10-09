export module platform;

import std;
import native_storage;

using namespace std;

export namespace platform {
	struct context {
		// Public part
		vector<string> args;

		// Native part: only implementations know the type stored here.
		native_storage<32> native;
	};

	void startup(context& ctx, const vector<string_view>& args);
	void shutdown(context& ctx);
}
