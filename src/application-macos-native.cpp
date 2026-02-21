module;

// #define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>
// #define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

export module application.macos.native;

import std;
import application;

using namespace std;

export namespace application {
	extern "C++" struct context {
		const CGRect frame = (CGRect){
			{	100.0, 100.0 },
			{ 1024.0, 768.0 }
		};
		NS::Application* pSharedApplication = nullptr;
		NS::Window* _pWindow = nullptr;

		function<void()> didFinishLaunching;
	};

	namespace native {
		CGRect get_frame(const context& ctx) {
			return ctx.frame;
		}
		NS::Window* get_window(context& ctx) {
			return ctx._pWindow;
		}
	}
}

extern "C++" namespace t {
	template<>
	template<>
	context_handle_t<application::context>::context_handle_t() {
		context_ = make_unique<application::context>();
	}
	template<>
	context_handle_t<application::context>::~context_handle_t() = default;
}
