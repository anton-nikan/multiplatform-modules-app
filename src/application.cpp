export module application;

export import context_handle;

export namespace application {
	struct context;
	using context_handle_t = t::context_handle_t<context>;

	void startup(context& ctx);
	void run(context& ctx);
	void shutdown(context& ctx);

	struct Rect {
		double x, y, width, height;
	};
	Rect get_frame(const context& ctx);
}
