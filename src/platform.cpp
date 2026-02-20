export module platform;

import std;
export import context_handle;

using namespace std;
export namespace platform {
	struct context;
	using context_handle_t = t::context_handle_t<context>;

	void startup(context& ctx, const vector<string_view>& args);
	void shutdown(context& ctx);
}
