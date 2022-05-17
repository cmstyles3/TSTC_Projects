#ifndef _BUTTON_H
#define _BUTTON_H

#include "../Engine/Engine.h"

class Button
{
	public:
		Button();
		~Button();

		void Init(LPDIRECT3DTEXTURE9 texture, int xLoc, int yLoc, int width, int height);
		void Kill();

		void SetEventFunc(void (*Func)());
		void SetPosition(int x, int y);
		void Update();
		void Draw();

		void SetEnable(bool enable);

		void IsOver();
		void IsClicked();

	private:
		void (*EventFunc)();

		LPDIRECT3DTEXTURE9 m_texture;
		int m_width, m_height;
		int m_xLoc, m_yLoc;
		D3DXVECTOR3 m_pos;
		bool m_enabled;
		bool m_isOver;
		bool m_isClicked;
		
};

#endif
