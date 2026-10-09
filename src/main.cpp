import platform;
import application;
import render;
import resources;

import std;
using namespace std;

int main(int argc, char** argv) {
	platform::context pctx;
	platform::startup(pctx, { argv, argv + argc });

	application::context appctx;
	application::startup(appctx);

	render::context rctx;
	appctx.did_finish_launching = [&] {
		render::startup(rctx, appctx);
	};

	auto model = resources::load("data/the_forgotten_knight-2.glb");
	if (model) {
		println(cout, "loaded!");
	}
	else {
		println(cerr, "{}", model.error());
	}

	application::run(appctx);

	render::shutdown(rctx);
	application::shutdown(appctx);
	platform::shutdown(pctx);

	return 0;
}
