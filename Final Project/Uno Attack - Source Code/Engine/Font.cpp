// FONT.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "engine.h"

//Font::Font()
//{
//	D3DXFONT_DESC fontDesc = {22,
//                          0,
//                          400,
//                          0,
//                          false,
//                          DEFAULT_CHARSET,
//                          OUT_TT_PRECIS,
//                          CLIP_DEFAULT_PRECIS,
//                          DEFAULT_PITCH,
//                          "Arial"};
//
//	D3DXCreateFontIndirect(g_engine->GetDevice(), &fontDesc, &m_font);
//}
//
//Font::~Font()
//{
//	SAFE_RELEASE(m_font);
//}
//
//void Font::OnLostDevice()
//{
//	m_font->OnLostDevice();
//}
//
//void Font::OnResetDevice()
//{
//	m_font->OnResetDevice();
//}
//
//void Font::Draw(int x, int y, char *text, unsigned long color)
//{
//	RECT position;
//	position.left = x;
//	position.top = y;
//	m_font->DrawText(NULL, text, -1, &position, DT_NOCLIP, color);
//}


Font::Font()
{
	m_fontTex = NULL;
	m_size = 0;
}

Font::Font(char *filename, int size)
{
	Init(filename, size);
}

Font::~Font()
{
	Kill();
}

void Font::Init(char *filename, int size)
{
	D3DXCreateTextureFromFile(g_engine->GetDevice(), filename, &m_fontTex);

	m_size = size;

	for (int i = 0; i < 224; i++)
	{
		int x = i%16;
		int y = i/16;
		int index = y * 16 + x;

		m_charRect[index].left   = (x * m_size);
		m_charRect[index].top    = (y * m_size);
		m_charRect[index].right  = m_charRect[index].left   + m_size;
		m_charRect[index].bottom = m_charRect[index].top + m_size;
	}

}

void Font::Kill()
{
	m_size = 0;
	SAFE_RELEASE(m_fontTex);
}

void Font::Draw(int x, int y, D3DCOLOR color, char fmt[], ...)
{
	char text[256];
	va_list ap;
	if (fmt == NULL)
		return;
	
	va_start(ap,fmt);
	vsprintf(text,fmt,ap);
	va_end(ap);

	int xPos = x, yPos = y;
	//g_engine->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
	for (int i = 0; i < strlen(text); i++)
	{
		if (text[i] == '\n')
		{
			xPos = x;
			yPos += m_size;
		}
		else
		{
			unsigned char ch = text[i] - 32;
			g_engine->GetSprite()->Draw(m_fontTex, &m_charRect[ch], &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(xPos, yPos, 0.0f), color);
			xPos += m_size / 2;
		}
	}
//	g_engine->GetSprite()->End();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////