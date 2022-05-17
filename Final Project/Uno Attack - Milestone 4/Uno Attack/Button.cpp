#include "Button.h"

Button::Button()
{
	EventFunc = NULL;
	m_texture = NULL;
	
}

Button::~Button()
{
	SAFE_RELEASE(m_texture);
}

void Button::Init(LPDIRECT3DTEXTURE9 texture, int xLoc, int yLoc, int width, int height)
{
	SAFE_RELEASE(m_texture);
	m_texture = texture;
	m_texture->AddRef();

	m_width = width;
	m_height = height;
	m_xLoc = xLoc;
	m_yLoc = yLoc;
}

void Button::Kill()
{
	SAFE_RELEASE(m_texture);
	EventFunc = NULL;
}

void Button::SetEventFunc(void (*Func)())
{
	EventFunc = Func;
}

void Button::SetPosition(int x, int y)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = 0.0f;
}

void Button::Update()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_pos.x && cursorPos.x < m_pos.x + m_width && 
		cursorPos.y > m_pos.y && cursorPos.y < m_pos.y + m_height)
	{
		if (m_isOver == false)
		{//play sound
		}
		m_isOver = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			m_isClicked = true;

		if (g_engine->GetInput()->PrevMouseButtonDown(0) &&
			!g_engine->GetInput()->MouseButtonDown(0))
		{
			EventFunc();
			m_isClicked = false;
		}
	}
	else
	{
		m_isOver = false;
		m_isClicked = false;
	}
}

void Button::Draw()
{
	RECT srcRect = {m_xLoc, m_yLoc, m_xLoc + m_width, m_yLoc + m_height};
	if (m_isClicked)
		g_engine->GetSprite()->Draw(m_texture, &srcRect, 0, &m_pos, 0xffff0000);
	else
		g_engine->GetSprite()->Draw(m_texture, &srcRect, 0, &m_pos, m_isOver ? 0xffffffff : 0xff888888);
}