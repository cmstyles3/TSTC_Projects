#ifndef _SPRITE__H
#define _SPRITE__H

#include <d3dx9.h>
#include "dxutil\dxutil.h"
#include "Vertex.h"

class CSprite
{
	struct Frame 
	{	
		RECT m_sourceRect;	
		D3DXVECTOR3 m_center;
	};

	public:

		CSprite();
		~CSprite();

		int Init(LPDIRECT3DDEVICE9 device, LPD3DXSPRITE sprite, int numFrames = 0);
		
		int Load(const char *filename);

		void Kill();

		void Draw();
		void Draw(int x, int y);
		void Draw(int frameIndex);
		void Draw(int frameIndex, int x, int y);
		
		void SetPosition(float x, float y, float z);
		void SetCenter(float x, float y);
		void SetSourceRect(int left, int top, int right, int bottom);
		void SetColor(D3DCOLOR color);

		void SetFrame(int frameIndex, int x, int y, int width, int height);
		
		RECT m_SourceRect;
		

	private:
		LPD3DXSPRITE m_pSprite;
		LPDIRECT3DTEXTURE9 m_pTexture;
		LPDIRECT3DDEVICE9 m_pDevice;
		D3DXVECTOR3 m_position;
		D3DXVECTOR3 m_center;
		RECT m_sourceRect;
		D3DCOLOR m_color;
		Frame *m_pFrames;
		int m_numFrames;
};

#endif _SPRITE__H