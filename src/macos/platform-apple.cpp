module;

#define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>

module platform;

import std;

using namespace std;

namespace platform {
	struct native_data {
		NS::AutoreleasePool* pAutoreleasePool = nullptr;
	};

	void startup(context& ctx, const vector<string_view>& args) {
		copy(begin(args), end(args), ostream_iterator<string_view>{ cout, "\n" });
		ctx.args.assign(begin(args), end(args));
		ctx.native.emplace<native_data>().pAutoreleasePool = NS::AutoreleasePool::alloc()->init();
	}

	void shutdown(context& ctx) {
		if (!ctx.native.has_value()) {
			return; // startup never ran
		}
		ctx.native.as<native_data>().pAutoreleasePool->release();
		ctx.native.reset();
	}
}
