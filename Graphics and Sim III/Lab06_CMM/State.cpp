#include "State.h"

State::State(DWORD id)
{
	MsgProc = NULL;
	m_id = id;
	m_color = 0xff000000;
}


DWORD State::GetID()
{
	return m_id;
}

DWORD State::GetColor()
{
	return m_color;
}