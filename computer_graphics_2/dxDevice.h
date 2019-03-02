#pragma once
#include "dxptr.h"
#include "window.h"
#include "dxStructures.h"
#include <vector>

class DxDevice
{
public:
	explicit DxDevice(const mini::Window& window);
	const mini::dx_ptr<ID3D11DeviceContext>& context() const
	{
		return m_context;
	}
	const mini::dx_ptr<IDXGISwapChain>& swapChain() const
	{
		return m_swapChain;
	}
	ID3D11Device* operator->() const
	{ 
		return m_device.get(); 
	}
	mini::dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(
		const mini::dx_ptr<ID3D11Texture2D>& texture) const;

	mini::dx_ptr<ID3D11Texture2D> CreateTexture(
		const D3D11_TEXTURE2D_DESC& desc) const;

	mini::dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
		const mini::dx_ptr<ID3D11Texture2D>& texture) const;

	mini::dx_ptr<ID3D11Buffer> CreateBuffer(const void* data,
		const D3D11_BUFFER_DESC& desc) const;

	static std::vector<BYTE> LoadByteCode(const std::wstring& filename);

	mini::dx_ptr<ID3D11VertexShader> CreateVertexShader(
		std::vector<BYTE> vsCode) const;

	mini::dx_ptr<ID3D11PixelShader> CreatePixelShader(
		std::vector<BYTE> psCode) const;

	mini::dx_ptr<ID3D11InputLayout> CreateInputLayout(
		const std::vector<D3D11_INPUT_ELEMENT_DESC> elements,
		std::vector<BYTE> vsCode) const;

private:
	mini::dx_ptr<ID3D11Device> m_device;
	mini::dx_ptr<ID3D11DeviceContext> m_context;
	mini::dx_ptr<IDXGISwapChain> m_swapChain;
};