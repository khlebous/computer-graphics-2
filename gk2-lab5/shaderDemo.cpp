#include "shaderDemo.h"

using namespace mini;
using namespace gk2;
using namespace DirectX;
using namespace std;
using namespace utils;

ShaderDemo::ShaderDemo(HINSTANCE hInst) : GK2ShaderDemoBase(hInst)
{
	//Shader Variables
	m_variables.AddSemanticVariable("modelMtx", VariableSemantic::MatM);
	m_variables.AddSemanticVariable("modelInvTMtx", VariableSemantic::MatMInvT);
	m_variables.AddSemanticVariable("viewProjMtx", VariableSemantic::MatVP);
	m_variables.AddSemanticVariable("camPos", VariableSemantic::Vec4CamPos);

	XMFLOAT4 lightPos[2] = { { -1,0,-1.7f,1 },{ 0,1.7f,0,1 } };
	XMFLOAT3 lightColor[2] = { { 1, 0.8f, 0.9f },{ 0.1f, 0, 1 } };
	m_variables.AddGuiVariable("lightPos", lightPos, -10, 10);
	m_variables.AddGuiColorsVariable("lightColor", lightColor);
	m_variables.AddGuiColorVariable("surfaceColor", XMFLOAT3{ 0.5f, 1.0f, 0.8f });
	m_variables.AddGuiVariable("ks", 0.8f);
	m_variables.AddGuiVariable("kd", 0.5f);
	m_variables.AddGuiVariable("ka", 0.2f);
	m_variables.AddGuiVariable("m", 50.f, 10.f, 200.f);

	//Teapot
	const auto teapot = addModelFromFile("models/Teapot.3ds");
	
	XMFLOAT4X4 modelMtx;
	float scale = 1.0f / 60.0f;
	auto h0 = 1.5f;
	XMStoreFloat4x4(&modelMtx, XMMatrixScaling(scale, scale, scale) *
		XMMatrixRotationX(-XM_PIDIV2) * XMMatrixTranslation(0.0f, 0.5f - h0, 0.0f));
	model(teapot).applyTransform(modelMtx);

	m_variables.AddSampler(m_device, "samp");
	m_variables.AddTexture(m_device, "normTex", L"textures/normal.png");

	// Spring
	m_variables.AddGuiVariable("h0", h0, 0, 3);
	m_variables.AddGuiVariable("l", 15.f, 5, 25);
	m_variables.AddGuiVariable("r", 0.5f, 0.01f, 1);
	m_variables.AddGuiVariable("rsmall", 0.1f, 0.01f, 0.5f);	const auto plane = addModelFromFile("models/Plane.obj");	XMStoreFloat4x4(&modelMtx, XMMatrixTranslation(0, -h0, 0));
	model(plane).applyTransform(modelMtx);

	//Render Passes
	const auto passTeapot = addPass(L"teapotVS.cso", L"teapotPS.cso");
	addModelToPass(passTeapot, teapot);
	const auto passSpring = addPass(L"springVS.cso", L"springPS.cso");
	addModelToPass(passSpring, plane);
}
