export module render;

export import application;
export import context_handle;

export namespace render {
	struct context;
	using context_handle_t = t::context_handle_t<context>;
	void startup(context& ctx, application::context& appctx);
	void shutdown(context& ctx);
}
