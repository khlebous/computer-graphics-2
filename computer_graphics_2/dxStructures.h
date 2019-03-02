#pragma once
#include <d3d11.h>

struct SwapChainDescription : DXGI_SWAP_CHAIN_DESC
{
	SwapChainDescription(HWND wndHwnd, SIZE wndSize);
};

struct Viewport : D3D11_VIEWPORT
{
	explicit Viewport(SIZE size);
};

struct Texture2DDescription : D3D11_TEXTURE2D_DESC
{
	Texture2DDescription(UINT width, UINT height);
};

struct BufferDescription : D3D11_BUFFER_DESC
{
	BufferDescription(UINT bindFlags, size_t byteWidth);
};