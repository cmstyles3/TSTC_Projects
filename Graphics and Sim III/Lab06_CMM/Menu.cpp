#include "menu.h"

MenuState::MenuState() : State(MENU_STATE)
{
	m_texture = 0;
	m_cursorTex = 0;	
}

void MenuState::Load()
{
	m_cursor = D3DXVECTOR2(300, 500);
	m_duckBtn = D3DXVECTOR2(6, 200);
	m_ballBtn = D3DXVECTOR2(272, 200);
	m_dartBtn = D3DXVECTOR2(538, 200);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "menu.bmp", &m_texture);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "cursor.dds", &m_cursorTex);
	
	
	m_btnSelectSound = new Sound("button4.wav");
	
}

void MenuState::Close()
{

	SAFE_DELETE(m_btnSelectSound);

	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_cursorTex);
}

void MenuState::Update(float dt)
{
	if (g_engine->GetInput()->KeyDown(DIK_ESCAPE) && !g_engine->GetInput()->PrevKeyDown(DIK_ESCAPE))
		PostQuitMessage(0);

	m_cursor.x += g_engine->GetInput()->MouseDX();
	m_cursor.y += g_engine->GetInput()->MouseDY();
	
	if (m_cursor.x < 0) m_cursor.x = 0;
	else if (m_cursor.x > g_engine->GetWidth() - 1) m_cursor.x = g_engine->GetWidth() - 1;
	if (m_cursor.y < 0) m_cursor.y = 0;
	else if (m_cursor.y > g_engine->GetHeight() - 1) m_cursor.y = g_engine->GetHeight() - 1;

	if (m_cursor.x >= m_duckBtn.x && m_cursor.x <= (m_duckBtn.x + 256) &&
		m_cursor.y >= m_duckBtn.y && m_cursor.y <= (m_duckBtn.y + 256))
	{
		if (m_duckBtnSelected == false)
			m_btnSelectSound->Play();
		m_duckBtnSelected = true;
		if (g_engine->GetInput()->MouseButtonDown(0))
			g_engine->ChangeState(DUCKSHOOTGAME_STATE);
	}
	else m_duckBtnSelected = false;

	if (m_cursor.x >= m_ballBtn.x && m_cursor.x <= (m_ballBtn.x + 256) &&
		     m_cursor.y >= m_ballBtn.y && m_cursor.y <= (m_ballBtn.y + 256))
	{
		if (m_ballBtnSelected == false)
			m_btnSelectSound->Play();
		m_ballBtnSelected = true;
		if (g_engine->GetInput()->MouseButtonDown(0))
			g_engine->ChangeState(BASEBALLTOSSGAME_STATE);
	}
	else m_ballBtnSelected = false;

	if (m_cursor.x >= m_dartBtn.x && m_cursor.x <= (m_dartBtn.x + 256) &&
		     m_cursor.y >= m_dartBtn.y && m_cursor.y <= (m_dartBtn.y + 256))
	{
		if (m_dartBtnSelected == false)
			m_btnSelectSound->Play();
		m_dartBtnSelected = true;
		if (g_engine->GetInput()->MouseButtonDown(0))
			g_engine->ChangeState(DARTTHROWGAME_STATE);
	}
	else m_dartBtnSelected = false;
}

void MenuState::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();
	sprite->Begin(0);

	// Draw background
	RECT rect = { 0, 0, 800, 600 };
	sprite->Draw(m_texture, 0, 0, 0, 0xffffffff);

	//Draw duck shoot button
	rect.left = 0;
	rect.right = 256;
	rect.top = 600;
	rect.bottom = 856;
	D3DXVECTOR3 position(6.0f, 200.0f, 0.0f);
	sprite->Draw(m_texture, &rect, 0, &position, m_duckBtnSelected ? 0xffffffff : 0xff888888);

	// Draw baseball toss button
	rect.left += 256;
	rect.right += 256;
	position.x += 266;
	sprite->Draw(m_texture, &rect, 0, &position, m_ballBtnSelected ? 0xffffffff : 0xff888888);

	// Draw dart throwing button
	rect.left += 256;
	rect.right += 256;
	position.x += 266;
	sprite->Draw(m_texture, &rect, 0, &position, m_dartBtnSelected ? 0xffffffff : 0xff888888);

	sprite->End();

	// Draw cursor
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	position.x = m_cursor.x;
	position.y = m_cursor.y;
	sprite->Draw(m_cursorTex, 0, 0, &position, 0xffffffff);
	sprite->End();

}