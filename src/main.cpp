import platform;
import application;
import render;
import resources;

import std;
using namespace std;

int main(int argc, char** argv) {
	platform::context_handle_t pctx{};
	platform::startup(pctx, { argv, argv + argc });

	application::context_handle_t appctx{};
	application::startup(appctx);

	render::context_handle_t rctx{};
	application::set_did_finish_launching(appctx, [&] {
		render::startup(rctx, appctx);
	});

	// // auto model = resources::load("the_forgotten_knight-2.glb");
	// // if (!holds_alternative<monostate>(model)) {
	// // 	println(cout, "loaded!");
	// // }

	application::run(appctx);

	// render::shutdown(rctx);
	application::shutdown(appctx);
	platform::shutdown(pctx);

	return 0;
}
