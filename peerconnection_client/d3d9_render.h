#ifndef _D3D9_RENDER_H_
#define _D3D9_RENDER_H_

#include <d3d9.h>
#include "api/video/video_frame.h"

class D3D9Render
{
public:
	int Init(HWND wnd, int width, int height);
	int Release();
	int UpdateYUV420(webrtc::I420BufferInterface* buffer);

private:
	HWND wnd_ = nullptr;
	int width_  = 0;
	int height_ = 0;
	RECT rect_;
	IDirect3D9 *d3d9_ = nullptr;
	IDirect3DDevice9* d3d9_device_ = nullptr;
	IDirect3DSurface9* d3d9_surface_ = nullptr;
};
	
#endif