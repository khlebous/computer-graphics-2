#include "environmentMapper.h"
#include "dxstructures.h"
#include "dxDevice.h"

using namespace mini;
using namespace gk2;
using namespace utils;
using namespace DirectX;
using namespace std;

const int EnvironmentMapper::TEXTURE_SIZE = 256;

EnvironmentMapper::EnvironmentMapper(const DxDevice& device, dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11PixelShader>&& ps,
	const ConstantBuffer<XMFLOAT4X4>& cbWorld, const ConstantBuffer<XMFLOAT4X4, 2>& cbView,
	const ConstantBuffer<XMFLOAT4X4>& cbProj, const ConstantBuffer<XMFLOAT4>& cbSurfaceColor,
	const dx_ptr<ID3D11SamplerState>& sampler, float nearPlane, float farPlane, XMFLOAT3 position)
	: BasicEffect(move(vs), move(ps)), VSConstantBuffers{ cbWorld, cbView, cbProj }, PSConstantBuffers{cbSurfaceColor},
	PSSamplers{sampler}, PSShaderResources(), m_nearPlane(nearPlane), m_farPlane(farPlane),
	m_position(position.x, position.y, position.z, 1.0f), m_face(static_cast<D3D11_TEXTURECUBE_FACE>(-1))
{
	Texture2DDescription texDesc;
	// TODO : 2.11 Setup texture width, height, mip levels and bind flags

	// TODO : 2.12 Uncomment following lines
	//m_faceTexture = device.CreateTexture(texDesc); 
	//m_renderTarget = device.CreateRenderTargetView(m_faceTexture);

	SIZE s;
	s.cx = s.cy = TEXTURE_SIZE;
	m_depthBuffer = device.CreateDepthStencilView(s);

	// TODO : 2.13 Create description for empty texture used as environment cube map, setup texture's width, height, mipLevels, bindflags, array size and miscFlags
	
	// TODO : 2.14 Uncomment following line
	//m_envTexture = device.CreateTexture(texDesc);

	// TODO : 2.15 Create description of shader resource view for cube map

	// TODO : 2.16 Uncomment following lines
	//auto envResourceView = device.CreateShaderResourceView(m_envTexture, srvDesc);
	//SetPSShaderResource(0, envResourceView);
}

void EnvironmentMapper::Begin(const dx_ptr<ID3D11DeviceContext>& context) const
{
	BasicEffect::Begin(context);
	VSConstantBuffers::Begin(context);
	PSConstantBuffers::Begin(context);
	PSSamplers::Begin(context);
	PSShaderResources::Begin(context);
}

void EnvironmentMapper::BeginFace(const dx_ptr<ID3D11DeviceContext>& context, ConstantBuffer<DirectX::XMFLOAT4X4, 2>& cbView, ConstantBuffer<DirectX::XMFLOAT4X4>& cbProj, D3D11_TEXTURECUBE_FACE face)
{
	m_face = face;

	// TODO : 2.17 Setup view matrix

	// TODO : 2.18 Replace with correct implementation
	XMMATRIX viewMtx = XMMatrixIdentity();

	XMFLOAT4X4 view[2];
	XMVECTOR det;
	XMStoreFloat4x4(view, viewMtx);
	XMStoreFloat4x4(view + 1, XMMatrixInverse(&det, viewMtx));
	cbView.Update(context, view);

	XMFLOAT4X4 proj;

	// TODO : 2.19 Replace with correct implementation
	XMStoreFloat4x4(&proj, XMMatrixIdentity());

	cbProj.Update(context, proj);

	D3D11_VIEWPORT viewport;

	// TODO : 2.20 Setup viewport

	context->RSSetViewports(1, &viewport);
	ID3D11RenderTargetView* targets[1] = { m_renderTarget.get() };
	context->OMSetRenderTargets(1, targets, m_depthBuffer.get());
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	context->ClearRenderTargetView(m_renderTarget.get(), clearColor);
	context->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void EnvironmentMapper::EndFace(const dx_ptr<ID3D11DeviceContext>& context)
{
	if (m_face < 0 || m_face > 5)
		return;

	// TODO : 2.21 Copy face to environment cube map

	m_face = static_cast<D3D11_TEXTURECUBE_FACE>(-1);
}