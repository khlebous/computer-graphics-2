#include "butterflyDemo.h"
#include "meshLoader.h"

using namespace mini;
using namespace utils;
using namespace gk2;
using namespace DirectX;
using namespace std;

#pragma region Constants
const float ButterflyDemo::DODECAHEDRON_R = sqrt(0.375f + 0.125f * sqrt(5.0f));
const float ButterflyDemo::DODECAHEDRON_H = 1.0f + 2.0f * DODECAHEDRON_R;
const float ButterflyDemo::DODECAHEDRON_A = XMScalarACos(-0.2f * sqrt(5.0f));

const float ButterflyDemo::MOEBIUS_R = 1.0f;
const float ButterflyDemo::MOEBIUS_W = 0.1f;
const unsigned int ButterflyDemo::MOEBIUS_N = 128;

const float ButterflyDemo::LAP_TIME = 10.0f;
const float ButterflyDemo::FLAP_TIME = 2.0f;
const float ButterflyDemo::WING_W = 0.15f;
const float ButterflyDemo::WING_H = 0.1f;
const float ButterflyDemo::WING_MAX_A = 8.0f * XM_PIDIV2 / 9.0f; //80 degrees

const unsigned int ButterflyDemo::BS_MASK = 0xffffffff;

const XMFLOAT4 ButterflyDemo::GREEN_LIGHT_POS = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
const XMFLOAT4 ButterflyDemo::BLUE_LIGHT_POS = XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f);
const XMFLOAT4 ButterflyDemo::COLORS[] = {
	XMFLOAT4(253.0f / 255.0f, 198.0f / 255.0f, 137.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(255.0f / 255.0f, 247.0f / 255.0f, 153.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(196.0f / 255.0f, 223.0f / 255.0f, 155.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(162.0f / 255.0f, 211.0f / 255.0f, 156.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(130.0f / 255.0f, 202.0f / 255.0f, 156.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(122.0f / 255.0f, 204.0f / 255.0f, 200.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(109.0f / 255.0f, 207.0f / 255.0f, 246.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(125.0f / 255.0f, 167.0f / 255.0f, 216.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(131.0f / 255.0f, 147.0f / 255.0f, 202.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(135.0f / 255.0f, 129.0f / 255.0f, 189.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(161.0f / 255.0f, 134.0f / 255.0f, 190.0f / 255.0f, 100.0f / 255.0f),
	XMFLOAT4(244.0f / 255.0f, 154.0f / 255.0f, 193.0f / 255.0f, 100.0f / 255.0f)
};
#pragma endregion

#pragma region Initalization
ButterflyDemo::ButterflyDemo(HINSTANCE hInstance)
	: Gk2ExampleBase(hInstance, 1280, 720, L"Motyl"),
	  m_cbWorld(m_device.CreateConstantBuffer<XMFLOAT4X4>()),
	  m_cbView(m_device.CreateConstantBuffer<XMFLOAT4X4, 2>()),
	  m_cbLighting(m_device.CreateConstantBuffer<Lighting>()),
	  m_cbSurfaceColor(m_device.CreateConstantBuffer<XMFLOAT4>())

{
	//Projection matrix
	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_projMtx, XMMatrixPerspectiveFovLH(XM_PIDIV4, ar, 0.01f, 100.0f));
	m_cbProj = m_device.CreateConstantBuffer<XMFLOAT4X4>(m_projMtx);
	XMFLOAT4X4 cameraMtx;
	XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
	UpdateCameraCB(cameraMtx);

	//Regular shaders
	auto vsCode = m_device.LoadByteCode(L"vs.cso");
	auto psCode = m_device.LoadByteCode(L"ps.cso");
	m_vs = m_device.CreateVertexShader(vsCode);
	m_ps = m_device.CreatePixelShader(psCode);
	m_il = m_device.CreateInputLayout(VertexPositionNormal::Layout, vsCode);

	//Bilboard shaders
	vsCode = m_device.LoadByteCode(L"vsBilboard.cso");
	psCode = m_device.LoadByteCode(L"psBilboard.cso");
	m_vsBilboard = m_device.CreateVertexShader(vsCode);
	m_psBilboard = m_device.CreatePixelShader(psCode);
	D3D11_INPUT_ELEMENT_DESC elements[1] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	m_ilBilboard = m_device.CreateInputLayout(elements, vsCode);

	//Render states
	CreateRenderStates();

	//Meshes
	vector<VertexPositionNormal> vertices;
	vector<unsigned short> indices;
	tie(vertices, indices) = MeshLoader::CreateBox();
	m_box = m_device.CreateMesh(indices, vertices);
	tie(vertices, indices) = MeshLoader::CreatePentagon();
	m_pentagon = m_device.CreateMesh(indices, vertices);
	tie(vertices, indices) = MeshLoader::CreateDoubleSidedSquare(2.0);
	m_wing = m_device.CreateMesh(indices, vertices);
	CreateMoebuisStrip();

	vector<XMFLOAT3> bilboardVertices;
	tie(bilboardVertices, indices) = MeshLoader::CreateSquareBilboard(2.0f);
	m_bilboard = m_device.CreateMesh(indices, bilboardVertices);

	//Model matrices
	CreateDodecahadronMtx();

	SetShaders();
	ID3D11Buffer* vsb[] = { m_cbWorld,  m_cbView, m_cbProj };
	m_device.context()->VSSetConstantBuffers(0, 3, vsb);
	ID3D11Buffer* psb[] = { m_cbSurfaceColor, m_cbLighting };
	m_device.context()->PSSetConstantBuffers(0, 2, psb);
}

void ButterflyDemo::CreateRenderStates()
//Setup render states used in various stages of the scene rendering
{
	DepthStencilDescription dssDesc;
	//TODO : 1.17. Setup depth stancil state for writing

	m_dssWrite = m_device.CreateDepthStencilState(dssDesc);

	//TODO : 1.18. Setup depth stencil state for testing
	
	m_dssTest = m_device.CreateDepthStencilState(dssDesc);

	//TODO : 1.10. Setup rasterizer state with ccw front faces

	m_rsCCW = m_device.CreateRasterizerState(RasterizerDescription());

	BlendDescription bsDesc;
	//TODO : 1.23. Setup alpha blending state

	m_bsAlpha = m_device.CreateBlendState(bsDesc);

	//TODO : 1.30. Setup additive blending state

	m_bsAdd = m_device.CreateBlendState(bsDesc);
}

void ButterflyDemo::CreateDodecahadronMtx()
//Compute dodecahedronMtx and mirrorMtx
{
	XMMATRIX mtx;

	XMStoreFloat4x4(&m_dodecahedronMtx[0],
		XMMatrixTranslation(0.0f, 0.0f, DODECAHEDRON_H / 2));

	mtx = XMLoadFloat4x4(&m_dodecahedronMtx[0]);
	XMStoreFloat4x4(&m_dodecahedronMtx[1], mtx *
		XMMatrixRotationZ(XM_PI) *
		XMMatrixRotationY(DODECAHEDRON_A - XM_PI));

	mtx = XMLoadFloat4x4(&m_dodecahedronMtx[1]);
	for (size_t i = 1; i <= 5; i++)
	{
		XMStoreFloat4x4(&m_dodecahedronMtx[i + 1], mtx *
			XMMatrixRotationZ(i * XM_2PI / 5.0f));
	}

	for (size_t i = 0; i < 6; i++)
	{
		mtx = XMLoadFloat4x4(&m_dodecahedronMtx[i]);
		XMStoreFloat4x4(&m_dodecahedronMtx[i + 6], mtx *
			XMMatrixRotationY(XM_PI)) ;
	}

	//TODO : 1.09. calcuate m_mirrorMtx matrices
}

XMFLOAT3 ButterflyDemo::MoebiusStripPos(float t, float s)
//TODO : 1.04. Compute the position of point on the Moebius strip for parameters t and s
{
	return {};
}

XMVECTOR ButterflyDemo::MoebiusStripDs(float t, float s)
//TODO : 1.05. Return the s-derivative of point on the Moebius strip for parameters t and s
{
	return {};
}

XMVECTOR ButterflyDemo::MoebiusStripDt(float t, float s)
//TODO : 1.06. Compute the t-derivative of point on the Moebius strip for parameters t and s
{
	return {};
}

void ButterflyDemo::CreateMoebuisStrip()
//TODO : 1.07. Create Moebius strip mesh
{

}
#pragma endregion

#pragma region Per-Frame Update
void ButterflyDemo::Update(const Clock& c)
{
	double dt = c.getFrameTime();
	if (HandleCameraInput(dt))
	{
		XMFLOAT4X4 cameraMtx;
		XMStoreFloat4x4(&cameraMtx, m_camera.getViewMatrix());
		UpdateCameraCB(cameraMtx);
	}
	UpdateButterfly(static_cast<float>(dt));
}

void ButterflyDemo::UpdateCameraCB(DirectX::XMFLOAT4X4 cameraMtx)
{
	XMMATRIX mtx = XMLoadFloat4x4(&cameraMtx);
	XMVECTOR det;
	auto invvmtx = XMMatrixInverse(&det, mtx);
	XMFLOAT4X4 view[2] = { cameraMtx };
	XMStoreFloat4x4(view + 1, invvmtx);
	m_cbView.Update(m_device.context(), view);
}

void ButterflyDemo::UpdateButterfly(float dtime)
//TODO : 1.26. Compute the matrices for butterfly wings. Position on the strip is determined based on time
{
	static float lap = 0.0f;
	lap += dtime;
	while (lap > LAP_TIME)
		lap -= LAP_TIME;
	//Value of the Moebius strip t parameter
	float t = 2 * lap / LAP_TIME;
	//Angle between wing current and vertical position
	float a = t * WING_MAX_A;
	t *= XM_2PI;
	if (a > WING_MAX_A)
		a = 2 * WING_MAX_A - a;
	//Write the rest of code here

}
#pragma endregion

#pragma region Frame Rendering Setup
void ButterflyDemo::SetShaders()
{
	m_device.context()->VSSetShader(m_vs.get(), 0, 0);
	m_device.context()->PSSetShader(m_ps.get(), 0, 0);
	m_device.context()->IASetInputLayout(m_il.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ButterflyDemo::SetBilboardShaders()
{
	m_device.context()->VSSetShader(m_vsBilboard.get(), 0, 0);
	m_device.context()->PSSetShader(m_psBilboard.get(), 0, 0);
	m_device.context()->IASetInputLayout(m_ilBilboard.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ButterflyDemo::Set1Light()
//Setup one positional light at the camera
{
	Lighting l{
		/*.ambientColor = */ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		/*.surface = */ XMFLOAT4(0.2f, 0.8f, 0.8f, 200.0f),
		/*.lights =*/ {
			{ /*.position =*/ m_camera.getCameraPosition(), /*.color =*/ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
			//other 2 lights set to 0
		}
	};
	m_cbLighting.Update(m_device.context(), l);
}

void ButterflyDemo::Set3Lights()
//Setup one white positional light at the camera
//TODO : 1.28. Setup two additional positional lights, green and blue.
{
	Lighting l{
		/*.ambientColor = */ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		/*.surface = */ XMFLOAT4(0.2f, 0.8f, 0.8f, 200.0f),
		/*.lights =*/{
			{ /*.position =*/ m_camera.getCameraPosition(), /*.color =*/ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
			//Write the rest of the code here

		}
	};

	//comment the following line when structure is filled
	ZeroMemory(&l.lights[1], sizeof(Light) * 2);

	m_cbLighting.Update(m_device.context(), l);
}
#pragma endregion

#pragma region Drawing
void ButterflyDemo::DrawBox()
{
	XMFLOAT4X4 worldMtx;
	XMStoreFloat4x4(&worldMtx, XMMatrixIdentity());
	m_cbWorld.Update(m_device.context(), worldMtx);
	m_box.Render(m_device.context());
}

void ButterflyDemo::DrawDodecahedron(bool colors)
//Draw dodecahedron. If color is true, use render faces with coresponding colors. Otherwise render using white color
{
	for (size_t i = 0; i < 12; i++)
	{
		m_cbSurfaceColor.Update(m_device.context(), COLORS[i]);
		m_cbWorld.Update(m_device.context(), m_dodecahedronMtx[i]);
		m_pentagon.Render(m_device.context());
	}

	//TODO : 1.11. Modify function so if colors parameter is set to false, all faces are drawn white instead

}

void ButterflyDemo::DrawMoebiusStrip()
//TODO : 1.08. Draw the Moebius strip mesh
{

}

void ButterflyDemo::DrawButterfly()
//TODO : 1.27. Draw the butterfly
{

}

void ButterflyDemo::DrawBilboards()
//Setup bilboards rendering and draw them
{
	//TODO : 1.33. Setup shaders and blend state

	//TODO : 1.34. Draw both bilboards with appropriate colors and transformations

	//TODO : 1.35. Restore rendering state to it's original values

}

void ButterflyDemo::DrawMirroredWorld(unsigned int i)
//Draw the mirrored scene reflected in the i-th dodecahedron face
{
	//TODO : 1.19. Setup render state for writing to the stencil buffer

	//TODO : 1.20. Draw the i-th face

	//TODO : 1.21. Setup depth stencil state for rendering mirrored world

	//TODO : 1.12. Setup rasterizer state and view matrix for rendering the mirrored world

	//TODO : 1.13. Draw objects of the mirrored scene - dodecahedron should be drawn with only one light and no colors and without blending

	//TODO : 1.14. Restore rasterizer state to it's original value

	//TODO : 1.36. Draw mirrored bilboards - they need to be drawn after restoring rasterizer state, but with mirrored view matrix

	//TODO : 1.15. Restore view matrix to its original value

	//TODO : 1.22. Restore depth stencil state to it's original value

}

void ButterflyDemo::Render()
{
	Gk2ExampleBase::Render();

	//render mirrored worlds
	for (int i = 0; i < 12; ++i)
		DrawMirroredWorld(i);

	//render dodecahedron with one light and alpha blending
	m_device.context()->OMSetBlendState(m_bsAlpha.get(), nullptr, BS_MASK);
	Set1Light();
	//TODO : 1.16. Comment the following line for now
	DrawDodecahedron(true);
	//TODO : 1.24. Uncomment the above line again
	m_device.context()->OMSetBlendState(nullptr, nullptr, BS_MASK);

	//render the rest of the scene with all lights
	Set3Lights();
	m_cbSurfaceColor.Update(m_device.context(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//TODO : 1.03. [optional] Comment the following line
	DrawBox();
	DrawMoebiusStrip();
	DrawButterfly();
	DrawBilboards();
}
#pragma endregion