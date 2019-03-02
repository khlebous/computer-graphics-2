#pragma once
#include "windowApplication.h"
#include "dxDevice.h"
#include "vertexPositionColor.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class DxApplication : public mini::WindowApplication
{
public:
	explicit DxApplication(HINSTANCE hInstance);
	std::vector<XMFLOAT2> CreateTriangleVertices();

protected:
	int MainLoop() override;

private:
	void Render();
	void Update();

	static std::vector<VertexPositionColor> CreateCubeVertices();
	static std::vector<unsigned short> CreateCubeIndices();
	
	DxDevice m_device; 
	DirectX::XMFLOAT4X4 m_modelMtx, m_viewMtx, m_projMtx;
	mini::dx_ptr<ID3D11Buffer> m_cbMVP;	mini::dx_ptr<ID3D11Buffer> m_indexBuffer;	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
	mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
	mini::dx_ptr<ID3D11InputLayout> m_layout;
};
