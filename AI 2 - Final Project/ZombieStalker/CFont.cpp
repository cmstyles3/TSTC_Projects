#include "CFont.h"

CFont::CFont()
{
	m_pDevice = 0;
	m_pSprite = 0;
	//m_pFonts = 0;
	//m_numFonts = 0;
	//m_currFont = 0;
}

CFont::~CFont()
{
	//SAFE_RELEASE(m_pDevice);
	//SAFE_RELEASE(m_pSprite);
	//SAFE_DELETE_ARRAY(m_pFonts);
}

void CFont::Init(LPDIRECT3DDEVICE9 pDevice, LPD3DXSPRITE pSprite, char filename[], int width, int height)
{
	
	/*m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pSprite = pSprite;
	m_pSprite->AddRef();*/

	/*if (numFonts < 1) 
		numFonts = 1;

	m_pFonts = new Font[numFonts];
	m_numFonts = numFonts;*/

	/*for (int i = 0; i < numFonts; i++)
	{
		m_pFonts[i].sprite.Init(pDevice, pSprite, 94);
		m_pFonts[i].width = 0;
		m_pFonts[i].height = 0;
	}*/

	m_sprite.Init(pDevice, pSprite, 94);
	m_sprite.Load(filename);
	m_width = width;
	m_height = height;
	for (int i = 0; i < 94; i++)
	{
		int x = i % 16;
		int y = i / 16;
		int index = (y * 16) + x;
		m_sprite.SetFrame(index, x * m_width, y * m_height, m_width, m_height);
		m_sprite.SetColor(D3DCOLOR_XRGB(255, 255, 255));
	}
}

//void CFont::Load(int fontIndex, char filename[], int width, int height)
//{
//	Font *font = &m_pFonts[fontIndex];
//	font->width = width;
//	font->height = height;
//	
//	font->sprite.Load(filename);
//
//	for (int i = 0; i < 94; i++)
//	{
//		int x = i % 16;
//		int y = i / 16;
//		int index = (y * 16) + x;
//		font->sprite.SetFrame(index, x * font->width, y * font->height, font->width, font->height);
//		font->sprite.SetColor(D3DCOLOR_XRGB(255, 255, 255));
//	}
//}
//
//void CFont::SetFont(int fontIndex)
//{
//	m_currFont = fontIndex;
//}

void CFont::Print(int x, int y, D3DCOLOR color, char fmt[], ...)
{
	char text[256];
	va_list ap;
	if (fmt == NULL)
		return;
	
	va_start(ap,fmt);
	vsprintf(text,fmt,ap);
	va_end(ap);

	/*Font *font = 0;
	font = &m_pFonts[m_currFont];*/
	
	m_sprite.SetColor(color);
	int xPos = x, yPos = y;
	for (int i = 0; i < strlen(text); i++)
	{
		if (text[i] == '\n')
		{
			xPos = x;
			yPos += m_height;
		}
		else
		{
			char ch = text[i] - 32;
			m_sprite.Draw(ch, xPos, yPos);
			xPos += m_width / 2;
		}
	}
}