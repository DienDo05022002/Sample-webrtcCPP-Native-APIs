#include "d3d9_render.h"
#include "rtc_base/logging.h"

int D3D9Render::Init(HWND wnd, int width, int height)
{
	this->Release();

	d3d9_ = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9_ == nullptr) {
		RTC_LOG(INFO) << "[D3D9Render] " << "Direct3DCreate9() failed()."; 
		return -1;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d9_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		wnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3d9_device_);
	if (FAILED(result)) {
		RTC_LOG(INFO) << "[D3D9Render] " << "CreateDevice() failed().";
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

int D3D9Render::UpdateYUV420(webrtc::I420BufferInterface* buffer)
{
	if (!d3d9_surface_ || !buffer) {
		return -1;
	}

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

	GetClientRect(wnd_, &rect_);

	IDirect3DSurface9 * back_buffer = nullptr;
	d3d9_device_->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
	d3d9_device_->StretchRect(d3d9_surface_, nullptr, back_buffer, &rect_, D3DTEXF_LINEAR);
	d3d9_device_->EndScene();
	d3d9_device_->Present(nullptr, nullptr, nullptr, nullptr);
	back_buffer->Release();

	return 0;
}