module;

// #define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>
// #define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

module application;

import std;
using namespace std;

class MyAppDelegate;

namespace application {
	struct context {
		const CGRect frame = (CGRect){
			{	100.0, 100.0 },
			{ 1024.0, 768.0 }
		};
		NS::Application* pSharedApplication = nullptr;
		NS::Window* _pWindow = nullptr;

		std::function<void()> didFinishLaunching;
	};

	void startup(context& ctx) {
		ctx.pSharedApplication = NS::Application::sharedApplication();
	}

	void run(context& ctx);

	void shutdown(context& ctx) {
	}

	Rect get_frame(const context& ctx) {
		return Rect{ .x = ctx.frame.origin.x, .y = ctx.frame.origin.y, .width = ctx.frame.size.width, .height = ctx.frame.size.height };
	}
}

class MyAppDelegate : public NS::ApplicationDelegate {
public:
	MyAppDelegate(application::context& ctx) : _ctx{ ctx } { }
	~MyAppDelegate() {
		_ctx._pWindow->release();
	}

	NS::Menu* createMenuBar() {
		using NS::StringEncoding::UTF8StringEncoding;

		NS::Menu* pMainMenu = NS::Menu::alloc()->init();
		NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
		NS::Menu* pAppMenu = NS::Menu::alloc()->init(NS::String::string("Appname", UTF8StringEncoding));

		NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
		NS::String* quitItemName = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(appName);
		SEL quitCb = NS::MenuItem::registerActionCallback("appQuit", [](void*, SEL, const NS::Object* pSender) {
			auto pApp = NS::Application::sharedApplication();
			pApp->terminate(pSender);
		});

		NS::MenuItem* pAppQuitItem = pAppMenu->addItem(quitItemName, quitCb, NS::String::string("q", UTF8StringEncoding));
		pAppQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
		pAppMenuItem->setSubmenu(pAppMenu);

		NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
		NS::Menu* pWindowMenu = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));

		SEL closeWindowCb = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*) {
			auto pApp = NS::Application::sharedApplication();
			pApp->windows()->object<NS::Window>(0)->close();
		});
		NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem(NS::String::string("Close Window", UTF8StringEncoding), closeWindowCb, NS::String::string("w", UTF8StringEncoding));
		pCloseWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

		pWindowMenuItem->setSubmenu(pWindowMenu);

		pMainMenu->addItem(pAppMenuItem);
		pMainMenu->addItem(pWindowMenuItem);

		pAppMenuItem->release();
		pWindowMenuItem->release();
		pAppMenu->release();
		pWindowMenu->release();

		return pMainMenu->autorelease();
	}

	void applicationWillFinishLaunching(NS::Notification* pNotification) override {
		NS::Menu* pMenu = createMenuBar();
		NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
		pApp->setMainMenu(pMenu);
		pApp->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
	}

	void applicationDidFinishLaunching(NS::Notification* pNotification) override {
		_ctx._pWindow = NS::Window::alloc()->init(
			_ctx.frame,
			NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
			NS::BackingStoreBuffered,
			false);

		_ctx._pWindow->setTitle(NS::String::string("Game", NS::StringEncoding::UTF8StringEncoding));

		_ctx._pWindow->makeKeyAndOrderFront(nullptr);

		NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
		pApp->activateIgnoringOtherApps(true);

		if (_ctx.didFinishLaunching) {
			_ctx.didFinishLaunching();
		}
	}

	bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) override {
		return true;
	}
private:
	application::context& _ctx;
};

namespace application {
	void run(context& ctx) {
		MyAppDelegate del{ ctx };
		ctx.pSharedApplication->setDelegate(&del);
		ctx.pSharedApplication->run();
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
