#include "Main.h"


void EventFunc()
{
	//MessageBox(0,"Event",0,0);
	g_engine->ChangeState(GS_SINGLEPLAYER);
}

void DoStuff()
{
	PostQuitMessage(0);
}

Menu::Menu(DWORD id) : State(id)
{
}

void Menu::Load()
{
	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/background.bmp", &m_bkgdTex);
	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/singleplayer.bmp", &m_spBtnTex);
	D3DXCreateTextureFromFileNonPow2(g_engine->GetDevice(), "Assets/Textures/cursor.dds", &m_cursorTex);

	m_button.Init(m_spBtnTex, 0, 0, 256, 50);
	m_button.SetPosition(380, 450);
	m_button.SetEventFunc(EventFunc);

	m_button2.Init(m_spBtnTex, 0, 0, 256, 50);
	m_button2.SetPosition(400, 510);
	m_button2.SetEventFunc(DoStuff);

	m_spBtnSelected = m_spBtnDown = false;
	m_spBtnWidth = 256;
	m_spBtnHeight = 50;
	m_spBtnPosition = D3DXVECTOR3(400.0f, 400.0f, 0.0f);

	m_cursorPosition = D3DXVECTOR3(500.0f, 240.0f, 0.0f);
}

void Menu::Close()
{
	m_button.Kill();
	m_button2.Kill();

	SAFE_RELEASE(m_bkgdTex);
	SAFE_RELEASE(m_spBtnTex);
	SAFE_RELEASE(m_cursorTex);
}

void Menu::OnLostDevice()
{
}

void Menu::OnResetDevice()
{
}

void Menu::Update(float dt)
{
	Input *input = g_engine->GetInput();
	if (input->KeyDown(DIK_ESCAPE) && 
		!input->PrevKeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	m_button.Update();
	//m_button2.Update();

}

void Menu::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();
	sprite->Begin(0);
	sprite->Draw(m_bkgdTex, 0, 0, 0, 0xffffffff);
	sprite->End();

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//sprite->Draw(m_spBtnTex, 0, 0, &m_spBtnPosition, m_spBtnSelected ? 0xffff00ff : 0xffffffff);
	m_button.Draw();
	//m_button2.Draw();
	sprite->Draw(m_cursorTex, 0, 0, &g_engine->GetInput()->GetCursorPos(), 0xffffffff);

	sprite->End();
}