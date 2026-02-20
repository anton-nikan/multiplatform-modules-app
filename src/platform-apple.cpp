module;

#define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>

module platform;

import std;
import context_handle;

using namespace std;

namespace platform {
	struct context {
		NS::AutoreleasePool* pAutoreleasePool = nullptr;
	};

	void startup(context& ctx, const vector<string_view>& args) {
		copy(begin(args), end(args), ostream_iterator<string_view>{ cout, "\n" });
		ctx.pAutoreleasePool = NS::AutoreleasePool::alloc()->init();
	}

	void shutdown(context& ctx) {
		ctx.pAutoreleasePool->release();
	}
}

extern "C++" namespace t {
	template<>
	template<>
	context_handle_t<platform::context>::context_handle_t() {
		context_ = make_unique<platform::context>();
	}
	template<>
	context_handle_t<platform::context>::~context_handle_t() = default;
}
