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
import application.macos.native;

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
	struct context {
		MTK::View* _pMtkView = nullptr;
		MTL::Device* _pDevice = nullptr;
		MTK::ViewDelegate* _pViewDelegate = nullptr;
	};

	void startup(context& ctx, application::context& appctx) {
		ctx._pDevice = MTL::CreateSystemDefaultDevice();

		const CGRect frame = application::native::get_frame(appctx);
		ctx._pMtkView = MTK::View::alloc()->init(frame, ctx._pDevice);
		ctx._pMtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
		ctx._pMtkView->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 1.0, 1.0));
		ctx._pMtkView->setDepthStencilPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);
		ctx._pMtkView->setClearDepth(1.0f);

		ctx._pViewDelegate = new MyRenderer(ctx._pDevice);
		ctx._pMtkView->setDelegate(ctx._pViewDelegate);

		application::native::get_window(appctx)->setContentView(ctx._pMtkView);
	}

	void shutdown(context& ctx) {
		ctx._pMtkView->release();
		ctx._pDevice->release();
		delete ctx._pViewDelegate;
	}
}

extern "C++" namespace t {
	template<>
	template<>
	context_handle_t<render::context>::context_handle_t() {
		context_ = make_unique<render::context>();
	}
	template<>
	context_handle_t<render::context>::~context_handle_t() = default;
}
