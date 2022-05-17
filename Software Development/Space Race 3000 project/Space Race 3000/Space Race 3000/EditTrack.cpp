#include "Main.h"


Track m_track;

EditTrack::EditTrack() : State(EDITTRACK_STATE)
{
	
}

void EditTrack::Load()
{
	m_track.color = D3DCOLOR_XRGB(140, 180, 255);
	m_track.terrainId = 0;
	m_track.rings.clear();

	m_track.color = D3DCOLOR_XRGB(m_red, m_green, m_blue);
	SetFog(10.0f, 100000.0f, m_track.color);
	m_color = m_track.color;

	m_camObj.Reset();
	m_camObj.SetPosition(D3DXVECTOR3(0.0f, 40000.0f, 0.0f));

}

void EditTrack::Close()
{
}

void EditTrack::OnResetDevice()
{
	SetFog(10.0f, 100000.0f, m_track.color);
}

void EditTrack::Update(float dt)
{
	Input *input = g_engine->GetInput();

	if (input->KeyDown(DIK_ESCAPE))
	{
		g_engine->ChangeState(MAINMENU_STATE);
	}

	static DWORD ringTimer = timeGetTime();
	if (input->KeyDown(DIK_SPACE) && ringTimer + 200 < timeGetTime())
	{
		Object3D ring;
		ring.SetPosition(m_camObj.GetPosition());
		ring.SetRight(m_camObj.GetRight());
		ring.SetUp(m_camObj.GetUp());
		ring.SetLook(m_camObj.GetLook());

		m_track.rings.push_back(ring);

		ringTimer = timeGetTime();
	}

	if (input->KeyDown(DIK_C))
	{
		m_track.rings.clear();
	}
	if ( input->KeyDown(DIK_L) &&
		!input->PrevKeyDown(DIK_L))
	{
		m_track.rings.pop_back();
	}

	bool colorChange = false;
	static DWORD redTimer = timeGetTime();
	if (input->KeyDown(DIK_1) && redTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_red--;
		redTimer = timeGetTime();
	}
	if (input->KeyDown(DIK_2) && redTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_red++;
		redTimer = timeGetTime();
	}

	static DWORD greenTimer = timeGetTime();
	if (input->KeyDown(DIK_3) && greenTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_green--;
		greenTimer = timeGetTime();
	}
	if (input->KeyDown(DIK_4) && greenTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_green++;
		greenTimer = timeGetTime();
	}

	if (m_red < 0) m_red = 0;
	else if (m_red > 255) m_red = 255;
	if (m_green < 0) m_green = 0;
	else if (m_green > 255) m_green = 255;
	if (m_blue < 0) m_blue = 0;
	else if (m_blue > 255) m_blue = 255;

	static DWORD blueTimer = timeGetTime();
	if (input->KeyDown(DIK_5) && greenTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_blue--;
		greenTimer = timeGetTime();
	}
	if (input->KeyDown(DIK_6) && greenTimer + 20 < timeGetTime())
	{
		colorChange = true;
		m_blue++;
		greenTimer = timeGetTime();
	}

	if (colorChange)
	{
		m_track.color = D3DCOLOR_XRGB(m_red, m_green, m_blue);
		m_color = m_track.color;
		SetFog(10.0f, 100000.0f, m_track.color);
	}


	static DWORD terrainTimer = timeGetTime();
	if (input->KeyDown(DIK_PGUP) && terrainTimer + 100 < timeGetTime())
	{
		m_track.terrainId--;
		terrainTimer = timeGetTime();
	}
	if (input->KeyDown(DIK_PGDN) && terrainTimer + 100 < timeGetTime())
	{
		m_track.terrainId++;
		terrainTimer = timeGetTime();
	}

	if (m_track.terrainId < 0)
		m_track.terrainId = 1;
	else if (m_track.terrainId > 1)
		m_track.terrainId = 0;

	if (input->KeyDown(DIK_F1) && !input->PrevKeyDown(DIK_F1))
	{
		SaveTrack(m_track, "new track.txt");
	}

	if (input->KeyDown(DIK_A))
	{
		m_camObj.Yaw(-1.0f * dt);
	}
	if (input->KeyDown(DIK_D))
	{
		m_camObj.Yaw(1.0f * dt);
	}
	if (input->KeyDown(DIK_W))
	{
		m_camObj.MoveOnZ(6000.0f * dt);
	}
	if (input->KeyDown(DIK_S))
	{
		m_camObj.MoveOnZ(-6000.0f * dt);
	}

	if (input->KeyDown(DIK_LEFT))
	{
		m_camObj.Roll(1.0f * dt);
	}
	if (input->KeyDown(DIK_RIGHT))
	{
		m_camObj.Roll(-1.0f * dt);
	}

	if (input->KeyDown(DIK_UP))
	{
		m_camObj.Pitch(1.0f * dt);
	}
	if (input->KeyDown(DIK_DOWN))
	{
		m_camObj.Pitch(-1.0f * dt);
	}


	D3DXVECTOR3 camPos = m_camObj.GetPosition();
	float terrainHeight = g_terrain[m_track.terrainId].GetHeight(camPos.x, camPos.z);
	float terrainMaxHeight = g_terrain[m_track.terrainId].GetMaxHeight();
	if (camPos.y < terrainHeight)
	{
		camPos.y = terrainHeight;
	}
	else if (camPos.y > terrainMaxHeight * 1.3f)
	{
		camPos.y = terrainMaxHeight * 1.3f;
	}

	float terrainWidth = g_terrain[m_track.terrainId].GetWidth();
	float terrainDepth = g_terrain[m_track.terrainId].GetDepth();

	if (camPos.x < -terrainWidth/2.0f)
	{
		camPos.x = -terrainWidth/2.0f;
	}
	else if (camPos.x > terrainWidth/2.0f)
	{
		camPos.x = terrainWidth/2.0f;
	}

	if (camPos.z < -terrainDepth/2.0f)
	{
		camPos.z = -terrainDepth/2.0f;
	}
	else if (camPos.z > terrainDepth/2.0f)
	{
		camPos.z = terrainDepth/2.0f;
	}

	m_camObj.SetPosition(camPos);

	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &m_camObj.GetViewMatrix());

}

void EditTrack::Draw()
{
	// Draw Terrain
	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);
	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &T);
	g_terrain[m_track.terrainId].Draw();

	// draw rings
	for (int i = 0; i < m_track.rings.size(); i++)
	{
		g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_track.rings[i].GetMatrix());
		g_ringMesh.Render();
	}

	g_font.Draw(0,0, 0xffffffff, "RGB(%i, %i, %i)", m_red, m_green, m_blue);
}