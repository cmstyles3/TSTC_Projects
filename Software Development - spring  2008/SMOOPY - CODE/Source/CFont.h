#ifndef _CFONT__H
#define _CFONT__H

#include <stdio.h>
#include "Sprite.h"

class CFont
{
	/*struct Font
	{
		CSprite sprite;
		int width;
		int height;
	};*/
	public:
		CFont();
		~CFont();
		void Init(LPDIRECT3DDEVICE9 pDevice, LPD3DXSPRITE pSprite, char filename[], int width, int height);
		//void Load(int fontIndex, char filename[], int width, int height);
		//void SetFont(int fontIndex);
		void Print(int x, int y, D3DCOLOR color, char fmt[],...);
	private:
		//Font *m_pFonts;
		LPD3DXSPRITE m_pSprite;
		LPDIRECT3DDEVICE9 m_pDevice;
		//int m_numFonts;
		//int m_currFont;
		CSprite m_sprite;
		int m_width;
		int m_height;
		
};


#endif