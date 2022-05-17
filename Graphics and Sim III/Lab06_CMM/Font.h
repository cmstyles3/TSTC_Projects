#ifndef FONT_H
#define FONT_H

#include "engine.h"

class Font
{
	public:
		Font();
		virtual ~Font();

		void OnLostDevice();
		void OnResetDevice();

		void Draw(int x, int y, char *text, unsigned long color);
	private:
		LPD3DXFONT m_font;
};

#endif