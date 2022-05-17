#include "Main.h"

void SetD3DOptions()
{
	// SET SAMPLING FILTER FOR TEXTURES TO LINEAR
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	g_engine->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_engine->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.25f, 800.0f/600.0f, 1.0f, 10000.0f);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &proj);

	D3DXVECTOR3 dir(0.0f, -1.0f, 0.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = InitDirectionalLight(&dir, &color);
	g_engine->GetDevice()->SetLight(0, &light);
	g_engine->GetDevice()->LightEnable(0, true);
}

void Init()
{
	g_engine->AddState(new TankTraining);
	SetD3DOptions();
}

void OnResetDevice()
{
	SetD3DOptions();
}
//void Kill()
//{
//}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	EngineSetup setup;
	ZeroMemory(&setup, sizeof(EngineSetup));
	setup.Init = Init;
	setup.OnResetDevice = OnResetDevice;
	setup.width = 800;
	setup.height = 600;
	//setup.Kill = Kill;
	setup.name = "Tank Training";
	setup.windowed = false;

	new Engine(&setup);
	g_engine->Run();

	return(1);
}