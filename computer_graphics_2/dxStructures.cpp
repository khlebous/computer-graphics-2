#include "dxStructures.h"

SwapChainDescription::SwapChainDescription(HWND wndHwnd, SIZE wndSize)
	: DXGI_SWAP_CHAIN_DESC{}
{
	BufferDesc.Width = wndSize.cx;
	BufferDesc.Height = wndSize.cy;
	BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// BufferDesc.RefreshRate.Numerator = 0;
	BufferDesc.RefreshRate.Denominator = 1;
	// BufferDesc.ScanlineOrdering =
	// DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED /*0*/;
	// BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED /*0*/;
	// SampleDesc.Quality = 0;
	SampleDesc.Count = 1;
	BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	BufferCount = 1;
	OutputWindow = wndHwnd;
	Windowed = true;
	// SwapEffect = DXGI_SWAP_EFFECT_DISCARD /*0*/;
	// Flags = 0;
}// komentarze - wartości domyśle są 0 i nie wymagają zmiany