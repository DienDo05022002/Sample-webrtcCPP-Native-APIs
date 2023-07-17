#include "d3d9_render.h"
#include "rtc_base/logging.h"

int D3D9Render::Init(HWND wnd, int width, int height)
{
	if (wnd == NULL) {
		return -1;
	}

	this->Release();

	if (!GetClientRect(wnd, &rect_)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "GetClientRect() failed().";
		return -1;
	}

	HRESULT result = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9_);
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "Direct3DCreate9Ex() failed().";
		return -1;
	}

	memset(&d3dpp_, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp_.Windowed = TRUE;
	d3dpp_.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp_.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp_.hDeviceWindow = wnd;
	d3dpp_.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp_.BackBufferWidth = rect_.right;
	d3dpp_.BackBufferHeight = rect_.bottom;
	d3dpp_.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp_.BackBufferCount = 1;

	display_width_ = rect_.right;
	display_height_ = rect_.bottom;

	int thread_modes[2] = { D3DCREATE_MULTITHREADED, 0 };
	int vertex_modes[4] = { D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
							 D3DCREATE_HARDWARE_VERTEXPROCESSING,
							 D3DCREATE_MIXED_VERTEXPROCESSING,
							 D3DCREATE_SOFTWARE_VERTEXPROCESSING };

	for (int t = 0; t < 2; t++) {
		for (int v = 0; v < 4; v++) {
			int creation_flags = thread_modes[t] | vertex_modes[v];
			result = d3d9_->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				wnd, creation_flags, &d3dpp_, nullptr, &d3d9_device_);
			if (SUCCEEDED(result)) {
				break;
			}
		}
	}

	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "CreateDevice() failed().";
		return -1;
	}

	result = d3d9_device_->SetMaximumFrameLatency(1);
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "SetMaximumFrameLatency() failed().";
		return -1;
	}

	result = d3d9_device_->CreateOffscreenPlainSurface(width, height, 
		(D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DPOOL_DEFAULT, 
		&d3d9_surface_, NULL);
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "CreateOffscreenPlainSurface() failed().";
		return -1;
	}

	wnd_ = wnd;
	width_ = width;
	height_ = height;
	return 0;
}

int D3D9Render::Release()
{
	if (d3d9_) {
		d3d9_->Release();
		d3d9_ = nullptr;
	}
		
	if (d3d9_device_) {
		d3d9_device_->Release();
		d3d9_device_ = nullptr;
	}
		
	if (d3d9_surface_) {
		d3d9_surface_->Release();
		d3d9_surface_ = nullptr;
	}
		
	width_ = 0;
	height_ = 0;
	memset(&rect_, 0, sizeof(RECT));
	return 0;
}

int D3D9Render::UpdateI420(webrtc::I420BufferInterface* buffer)
{
	if (!d3d9_surface_ || !buffer) {
		return -1;
	}

	RECT src_rect;
	RECT dst_rect;
	GetClientRect(wnd_, &rect_);

	if (display_width_ != rect_.right || display_height_ != rect_.bottom) {
		d3dpp_.BackBufferWidth = rect_.right;
		d3dpp_.BackBufferHeight = rect_.bottom;
		if (d3d9_device_->ResetEx(&d3dpp_, NULL) != S_OK) {
			return -1;
		}
		display_width_ = d3dpp_.BackBufferWidth;
		display_height_ = d3dpp_.BackBufferHeight;
	}

	src_rect.left = 0;
	src_rect.right = buffer->width();
	src_rect.top = 0;
	src_rect.bottom = buffer->height();

	rect_.left = dst_rect.left = 0;
	rect_.right = dst_rect.right = display_width_;
	rect_.top = dst_rect.top = 0;
	rect_.bottom = dst_rect.bottom = display_height_;

#if 1
	int dst_h = rect_.right * src_rect.bottom / src_rect.right;
	int dst_w = rect_.bottom * src_rect.right / src_rect.bottom;
	if (dst_h > rect_.bottom) {
		rect_.left = (rect_.right - dst_w) / 2;
		rect_.right = dst_w;
	}
	else {
		rect_.top = (rect_.bottom - dst_h) / 2;
		rect_.bottom = dst_h;
	}
#endif

	dst_rect.left = rect_.left;	
	dst_rect.right = rect_.left + rect_.right;
	dst_rect.top = rect_.top;
	dst_rect.bottom = rect_.top + rect_.bottom;

	D3DLOCKED_RECT d3d_rect;
	HRESULT result = d3d9_surface_->LockRect(&d3d_rect, nullptr, D3DLOCK_DONOTWAIT);
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "LockRect() failed().";
		return -1;
	}

	char* dst = (char *)d3d_rect.pBits;
	char* srcY = (char *)buffer->DataY();
	char* srcU = (char *)buffer->DataU();
	char* srcV = (char *)buffer->DataV();
	int stride = d3d_rect.Pitch;
	int strideY = buffer->StrideY();
	int strideU = buffer->StrideU();
	int strideV = buffer->StrideV();

	for (int i = 0; i < height_; i++) {
		memcpy(dst + i * stride, srcY + i * strideY, width_);
	}

	dst += stride * height_;
	for (int i = 0; i < height_ / 2; i++) {
		memcpy(dst + i * stride / 2, srcV + i * strideV, width_ / 2);
	}

	dst += stride * height_ / 4;
	for (int i = 0; i < height_ / 2; i++) {
		memcpy(dst + i * stride / 2, srcU + i * strideU, width_ / 2);
	}

	result = d3d9_surface_->UnlockRect();
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "UnlockRect() failed().";
		return -1;
	}

	d3d9_device_->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3d9_device_->BeginScene();
	IDirect3DSurface9 * back_buffer = nullptr;
	
	d3d9_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
	d3d9_device_->StretchRect(d3d9_surface_, &src_rect, back_buffer, &dst_rect, D3DTEXF_LINEAR);
	d3d9_device_->EndScene();

	HRESULT hr = S_OK;
	do {
		HRESULT hr = d3d9_device_->PresentEx(nullptr, nullptr, nullptr, nullptr, 0);
		if (hr == D3DERR_WASSTILLDRAWING) {
			Sleep(1);
		}
	} while (hr == D3DERR_WASSTILLDRAWING);

	if (back_buffer != nullptr) {
		back_buffer->Release();
	}

	return 0;
}