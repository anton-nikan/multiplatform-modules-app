export module application;

import std;
export import context_handle;

using namespace std;

export namespace application {
	extern "C++" struct context;
	using context_handle_t = t::context_handle_t<context>;

	void startup(context& ctx);
	void run(context& ctx);
	void shutdown(context& ctx);

	void set_did_finish_launching(context& ctx, function<void()> f);
}
