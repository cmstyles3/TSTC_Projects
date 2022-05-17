#include "Font.h"

#include "engine.h"

Font::Font()
{
	D3DXFONT_DESC fontDesc = {24,
                          0,
                          400,
                          0,
                          false,
                          DEFAULT_CHARSET,
                          OUT_TT_PRECIS,
                          CLIP_DEFAULT_PRECIS,
                          DEFAULT_PITCH,
                          "Arial"};

	D3DXCreateFontIndirect(g_engine->GetDevice(), &fontDesc, &m_font);
}

Font::~Font()
{
	SAFE_RELEASE(m_font);
}

void Font::OnLostDevice()
{
	m_font->OnLostDevice();
}

void Font::OnResetDevice()
{
	m_font->OnResetDevice();
}

void Font::Draw(int x, int y, char *text, unsigned long color)
{
	RECT position;
	position.left = x;
	position.top = y;
	m_font->DrawText(NULL, text, -1, &position, DT_NOCLIP, color);
}