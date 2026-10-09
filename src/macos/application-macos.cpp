module;

// #define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>
// #define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>

module application;

import std;
import application.native;

using namespace std;

class MyAppDelegate;

namespace application {
	void startup(context& ctx) {
		ctx.native.emplace<native::data>().pSharedApplication = NS::Application::sharedApplication();
	}

	void run(context& ctx);

	void shutdown(context& ctx) {
	}
}

class MyAppDelegate : public NS::ApplicationDelegate {
public:
	MyAppDelegate(application::context& ctx) : _ctx{ ctx } { }
	~MyAppDelegate() {
		if (_ctx.native.has_value()) {
			auto& n = _ctx.native.as<application::native::data>();
			if (n._pWindow != nullptr) {
				n._pWindow->release();
				n._pWindow = nullptr;
			}
		}
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
		_ctx.native.as<application::native::data>()._pWindow = NS::Window::alloc()->init(
			application::native::make_frame(_ctx.frame.x, _ctx.frame.y, _ctx.frame.width, _ctx.frame.height),
			NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
			NS::BackingStoreBuffered,
			false);

		_ctx.native.as<application::native::data>()._pWindow->setTitle(NS::String::string("Game", NS::StringEncoding::UTF8StringEncoding));

		_ctx.native.as<application::native::data>()._pWindow->makeKeyAndOrderFront(nullptr);

		NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
		pApp->activateIgnoringOtherApps(true);

		if (_ctx.did_finish_launching) {
			_ctx.did_finish_launching();
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
		ctx.native.as<native::data>().pSharedApplication->setDelegate(&del);
		ctx.native.as<native::data>().pSharedApplication->run();
		// The delegate lives on this stack frame, don't leave a dangling pointer behind.
		ctx.native.as<native::data>().pSharedApplication->setDelegate(nullptr);
	}
}
