module;

#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>

export module application.native;

export namespace application::native {
	struct data {
		NS::Application* pSharedApplication = nullptr;
		NS::Window* _pWindow = nullptr;
	};

	CGRect make_frame(double x, double y, double width, double height) {
		return CGRect{ { x, y }, { width, height } };
	}
}
