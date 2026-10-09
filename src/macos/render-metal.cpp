module;

#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <MetalKit/MetalKit.hpp>

#include <simd/simd.h>

module render;

import std;
import application;
import application.native;

using namespace std;

constexpr int kMaxFramesInFlight = 3;

class MyRenderer : public MTK::ViewDelegate {
public:
	MyRenderer(MTL::Device* pDevice) : MTK::ViewDelegate(), _pDevice{ pDevice->retain() } {
		_pCommandQueue = _pDevice->newCommandQueue();
		_semaphore = dispatch_semaphore_create(kMaxFramesInFlight);
	}
	~MyRenderer() override {
		_pCommandQueue->release();
		_pDevice->release();
	}

	void drawInMTKView(MTK::View* pView) override {
		MTK::ViewDelegate::drawInMTKView(pView);

		using simd::float3;
		using simd::float4;
		using simd::float4x4;

		NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

		_frame = (_frame + 1) % kMaxFramesInFlight;

		MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
		dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);

		auto* pRenderer = this;
		pCmd->addCompletedHandler(^void(MTL::CommandBuffer* pCmd) {
			dispatch_semaphore_signal(pRenderer->_semaphore);
		});

		// Begin render pass:
		MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
		MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);

		pEnc->setCullMode(MTL::CullModeBack);
		pEnc->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

		pEnc->endEncoding();

		pCmd->presentDrawable(pView->currentDrawable());
		pCmd->commit();

		pPool->release();
	}
private:
	MTL::Device* _pDevice = nullptr;
	MTL::CommandQueue* _pCommandQueue = nullptr;
	int _frame = 0;
	dispatch_semaphore_t _semaphore;
};

namespace render {
	struct native_data {
		MTK::View* _pMtkView = nullptr;
		MTL::Device* _pDevice = nullptr;
		MTK::ViewDelegate* _pViewDelegate = nullptr;
	};
	void startup(context& ctx, application::context& appctx) {
		auto& n = ctx.native.emplace<native_data>();
		n._pDevice = MTL::CreateSystemDefaultDevice();

		const application::rect& r = appctx.frame;
		const CGRect frame = application::native::make_frame(r.x, r.y, r.width, r.height);
		n._pMtkView = MTK::View::alloc()->init(frame, n._pDevice);
		n._pMtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		n._pMtkView->setClearColor(MTL::ClearColor::Make(ctx.clear_color[0], ctx.clear_color[1], ctx.clear_color[2], ctx.clear_color[3]));
		n._pMtkView->setDepthStencilPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);
		n._pMtkView->setClearDepth(1.0f);

		n._pViewDelegate = new MyRenderer(n._pDevice);
		n._pMtkView->setDelegate(n._pViewDelegate);

		appctx.native.as<application::native::data>()._pWindow->setContentView(n._pMtkView);
	}

	void shutdown(context& ctx) {
		if (!ctx.native.has_value()) {
			return; // startup never ran
		}
		auto& n = ctx.native.as<native_data>();
		n._pMtkView->release();
		n._pDevice->release();
		delete n._pViewDelegate;
		ctx.native.reset();
	}
}
