#include "Main.h"
#include "dsound.h"

TankTraining::TankTraining() : State(TANKTRAINING_STATE)
{
}

void TankTraining::Load()
{
	m_currState = PLAY;

	m_color = 0xff555555;
	SetFog(1.0f, 200.0f, m_color);

	m_terrain = new Terrain(257, 257, 2.0f, 2.0f, 
		"Assets/Heightmap/Heightmap.raw", 
		"Assets/Textures/grass512.bmp", 
		"Assets/Textures/dirt512.bmp", 
		"Assets/Textures/sand512.bmp", 
		"Assets/Textures/sand512.bmp",
		D3DCOLOR_XRGB(0, 60, 130),
		0.1f, 0.0f);

	m_terrain->GenTexture(2048, 2048, "Assets/Textures/TerrainTexture.bmp");

	m_tankMesh.LoadXFile("Assets/Models/tank.x");

	m_currObj = TREE;
	m_objMeshes[0].LoadXFile("Assets/Models/tree1.x");
	m_objMeshes[1].LoadXFile("Assets/Models/building.x");
	m_objMeshes[1].Scale(0.10f);

	m_font.Init("Assets/Textures/Font.dds", 32);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/rain.dds", &m_rainTex);
	
	LoadObjects();

	//Initialize rain drops
	for (int i = 0; i < MAX_RAINDROP; i++)
	{
		m_rainDrops[i].pos = D3DXVECTOR3(rand()%800, rand()%600, 0.0f);
		m_rainDrops[i].vel = D3DXVECTOR3(0.0f, rand()%200+900, 0.0f);
	}

	//D3DXCreateSphere(g_engine->GetDevice(), 0.2f, 10, 10, &m_shellMesh, 0);
	D3DXCreateCylinder(g_engine->GetDevice(), 0.1f, 0.15f, 3.0f, 10, 1, &m_shellMesh, 0);
	ZeroMemory(&m_shells, sizeof(Shell) * MAX_SHELLS);

	InitDirectSound();
	LoadWAV("Assets/Sounds/lightrain.wav", 0);
	LoadWAV("Assets/Sounds/thunderrain.wav", 1);
	LoadWAV("Assets/Sounds/cannon.wav", 2);
	LoadWAV("Assets/Sounds/tank.wav", 3);
	Play(0, true);

	m_displayControls = false;

}

void TankTraining::Close()
{
	SAFE_DELETE(m_terrain);

	m_tankMesh.Kill();

	for (int i = 0; i < NUM_OBJECTS; i++)
		m_objMeshes[i].Kill();

	//SAFE_DELETE(m_sounds[0]);

	m_font.Kill();

	m_font.Kill();
	SAFE_RELEASE(m_rainTex);
	SAFE_RELEASE(m_shellMesh);

	KillDirectSound();
}

void TankTraining::OnResetDevice()
{
	SetFog(1.0f, 200.0f, m_color);
}

void TankTraining::Update(float dt)
{
	Input *input = g_engine->GetInput();

	D3DXVECTOR3 oldTankPos = m_tankObj.GetPosition();

	static DWORD timer = timeGetTime();
	if (rand()%1000 == 0 && timer + 8000 < timeGetTime())
	{
		Play(1);
		timer = timeGetTime();
	}

	if (input->KeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	static bool tankMoved = false;
	

	if (input->KeyDown(DIK_UP))
	{
		if (tankMoved == false)
			Play(3, true);
		tankMoved = true;

		m_camera.Walk(20.0f * dt);
		m_tankObj.MoveOnZ(20.0f * dt);
	}
	else if (input->KeyDown(DIK_DOWN))
	{
		if (tankMoved == false)
			Play(3, true);
		tankMoved = true;
		m_camera.Walk(-20.0f * dt);
		m_tankObj.MoveOnZ(-20.0f * dt);
	}
	else 
	{
		if (tankMoved)
			Stop(3);
		tankMoved = false;

	}

	if (tankMoved)
	{
		if (input->KeyDown(DIK_LEFT))
		{
			m_tankObj.Yaw(-1.0f * dt);
		}
		if (input->KeyDown(DIK_RIGHT))
		{
			m_tankObj.Yaw(1.0f * dt);
		}
	}

	switch(m_currState)
	{
		case PLAY:
		{
			if (input->KeyDown(DIK_A))
			{
				m_camera.Strafe(-150.0f * dt);
			}
			if (input->KeyDown(DIK_D))
			{
				m_camera.Strafe(150.0f * dt);
			}
			if (input->KeyDown(DIK_W))
			{
				m_camera.Fly(100.0f * dt);
			}
			if (input->KeyDown(DIK_C) &&
				!input->PrevKeyDown(DIK_C))
			{
				m_displayControls = !m_displayControls;
			}
			static DWORD shootTimer = timeGetTime();
			if (input->KeyDown(DIK_SPACE) && shootTimer + 1000 < timeGetTime())
			{
				Play(2);
				ShootShell();
				shootTimer = timeGetTime();
			}
			/*if (input->KeyDown(DIK_E))
			{
				m_currState = EDIT;
			}*/
		}break;

		case EDIT:
		{
			if (input->KeyDown(DIK_P))
			{
				m_currState = PLAY;
			}

			if (input->KeyDown(DIK_1) &&
				!input->PrevKeyDown(DIK_1))
			{
				m_currObj--;
			}
			if (input->KeyDown(DIK_2) &&
				!input->PrevKeyDown(DIK_2))
			{
				m_currObj++;
			}
			if (m_currObj < 0) m_currObj = NUM_OBJECTS-1;
			else if (m_currObj >= NUM_OBJECTS) m_currObj = 0;

			if (input->KeyDown(DIK_SPACE) &&
				!input->PrevKeyDown(DIK_SPACE))
			{
				Object obj;
				obj.pos = m_tankObj.GetPosition();
				obj.pos.y -= 2;
				D3DXMatrixTranslation(&obj.trans, obj.pos.x, obj.pos.y, obj.pos.z);
				obj.id = m_currObj;
				m_objects.push_back(obj);
			}
			if (input->KeyDown(DIK_S) &&
				!input->PrevKeyDown(DIK_S))
			{
				SaveObjects();
			}
		}break;
	}

	for (int i = 0; i < MAX_RAINDROP; i++)
	{
		m_rainDrops[i].pos += m_rainDrops[i].vel * dt;

		if (m_rainDrops[i].pos.y > 600)
		{
			m_rainDrops[i].pos = D3DXVECTOR3(rand()%800, 0, 0.0f);
			m_rainDrops[i].vel = D3DXVECTOR3(0.0f, rand()%200+900, 0.0f);
		}
	}

	UpdateShells(dt);


	D3DXVECTOR3 tankPos = m_tankObj.GetPosition();

	D3DXVECTOR3 tankFrontLeftPos = m_tankObj.GetPosition() + m_tankObj.GetLook() * 3.0f - m_tankObj.GetRight() * 2.0f - m_tankObj.GetUp() * .15f;
	D3DXVECTOR3 tankFrontRightPos = m_tankObj.GetPosition() + m_tankObj.GetLook() * 3.0f + m_tankObj.GetRight() * 2.0f - m_tankObj.GetUp() * .15f;
	D3DXVECTOR3 tankBackLeftPos = m_tankObj.GetPosition() - m_tankObj.GetLook() * 3.0f - m_tankObj.GetRight() * 2.0f - m_tankObj.GetUp() * .15f;
	D3DXVECTOR3 tankBackRightPos = m_tankObj.GetPosition() - m_tankObj.GetLook() * 3.0f + m_tankObj.GetRight() * 2.0f - m_tankObj.GetUp() * .15f;

	float tankHeight = m_terrain->GetHeight(tankPos.x, tankPos.z);

	float frontLeftHeight = m_terrain->GetHeight(tankFrontLeftPos.x, tankFrontLeftPos.z);
	float frontRightHeight = m_terrain->GetHeight(tankFrontRightPos.x, tankFrontRightPos.z);
	float backLeftHeight = m_terrain->GetHeight(tankBackLeftPos.x, tankBackLeftPos.z);
	float backRightHeight = m_terrain->GetHeight(tankBackRightPos.x, tankBackRightPos.z);

	float heightAvg = (tankHeight + frontLeftHeight+frontRightHeight+backLeftHeight+backRightHeight)/5.0f;

	D3DXVECTOR3 frontLeftNormal = m_terrain->GetNormal(tankFrontLeftPos.x, tankFrontLeftPos.z);
	D3DXVECTOR3 frontRightNormal = m_terrain->GetNormal(tankFrontRightPos.x, tankFrontRightPos.z);
	D3DXVECTOR3 backLeftNormal = m_terrain->GetNormal(tankBackLeftPos.x, tankBackLeftPos.z);
	D3DXVECTOR3 backRightNormal = m_terrain->GetNormal(tankBackRightPos.x, tankBackRightPos.z); 
	D3DXVECTOR3 normal = m_terrain->GetNormal(tankPos.x, tankPos.z);
	D3DXVECTOR3 normalAvg = (normal+frontLeftNormal+frontRightNormal+backLeftNormal+backRightNormal)/5.0f;

	tankPos.y = heightAvg;
	m_tankObj.SetUp(normalAvg);
	m_tankObj.SetOffset(normalAvg*0.15f);
	m_tankObj.SetPosition(tankPos);
	
	/*D3DXVECTOR3 tankNormal = m_terrain->GetNormal(tankPos.x, tankPos.z);
	m_tankObj.SetUp(tankNormal);
	tankPos.y = terrainHeight;
	m_tankObj.SetOffset(tankNormal*0.15f);
	m_tankObj.SetPosition(tankPos);*/

	if (m_currState == PLAY)
	{
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i].id == TREE)
			{
				if (fabsf(tankPos.x - m_objects[i].pos.x) < 3.8f &&
					fabsf(tankPos.z - m_objects[i].pos.z) < 3.8f)
				{
					m_tankObj.SetPosition(oldTankPos);
				}
			}
		}
	}

	D3DXMATRIX V;
	m_camera.SetLook(m_tankObj.GetLook());
	m_camera.SetRight(m_tankObj.GetRight());
	m_camera.SetUp(m_tankObj.GetUp());
	D3DXVECTOR3 targetPos = (m_tankObj.GetPosition() - m_tankObj.GetLook() * 20.0f + m_tankObj.GetUp() * 6.0f);
	m_camera.UpdateThirdPerson(targetPos, &V, dt);
	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
}

void TankTraining::Draw()
{
	DrawTerrain();

	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_tankObj.GetMatrix());
	m_tankMesh.Render();

	DrawShells();

	g_engine->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	g_engine->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	g_engine->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 200);
	for (int i = 0; i < m_objects.size(); i++)
	{
		D3DXMATRIX T;
		if (m_objects[i].id == TREE)
		{
			D3DXMATRIX S;
			D3DXMatrixScaling(&S, 0.5f, 0.5f, 0.5f);
			T = S * m_objects[i].trans;
		}
		else
		{
			T = m_objects[i].trans;
		}
		g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &T);
		m_objMeshes[m_objects[i].id].Render();
	}
	g_engine->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);

	g_engine->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	for (int i = 0; i < MAX_RAINDROP; i++)
		g_engine->GetSprite()->Draw(m_rainTex, 0, 0, &m_rainDrops[i].pos, 0xffffffff);

	g_engine->GetSprite()->End();
	
	if (m_displayControls)
	{
		m_font.Draw(0,0, 0xffffffff, "Press up arrow to move tank forward\n\
Press down arrow to move tank backward\n\
Press left and right arrows to steer tank\n\
Press and hold 'A' to move camera left\n\
Press and hold 'D' to move camera right\n\
Press and hold 'W' to move camera up\n\
Press the space bar to fire tank cannon");
	}
	else m_font.Draw(0,0, 0xffffffff, "Press 'C' to display Controls");

	/*switch(m_currState)
	{
		case PLAY:
		{
			m_font.Draw(0,0,0xffffffff, "PLAY MODE");
		}break;

		case EDIT:
		{
			m_font.Draw(0,0,0xffffffff, "EDIT MODE");
		}break;
	}*/
	
}


void TankTraining::DrawTerrain()
{
	D3DXMATRIX t;
	D3DXMatrixIdentity(&t);
	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &t);
	m_terrain->Draw();
}

void TankTraining::SaveObjects()
{
	ofstream outfile;
	outfile.open("Objects.txt");
	outfile << m_objects.size() << endl;
	for (int i = 0; i < m_objects.size(); i++)
	{
		outfile << m_objects[i].id << endl;
		outfile << m_objects[i].pos.x << " " 
			<< m_objects[i].pos.y << " " 
			<< m_objects[i].pos.z << endl;
	}
	outfile.close();
}

void TankTraining::LoadObjects()
{
	ifstream infile;
	infile.open("Objects.txt");
	int size;
	infile >> size;
	m_objects.resize(size);
	for (int i = 0; i < m_objects.size(); i++)
	{
		infile >> m_objects[i].id;
		infile >> m_objects[i].pos.x 
			>> m_objects[i].pos.y 
			>> m_objects[i].pos.z;

		D3DXMatrixTranslation(&m_objects[i].trans, m_objects[i].pos.x, m_objects[i].pos.y, m_objects[i].pos.z);
	}
	infile.close();
}

void TankTraining::ShootShell()
{
	for (int i = 0; i < MAX_SHELLS; i++)
	{
		if (!m_shells[i].active)
		{
			m_shells[i].active = true;

			D3DXVECTOR3 tankPos = m_tankObj.GetPosition();
			D3DXVECTOR3 tankLook = m_tankObj.GetLook();
			D3DXVECTOR3 tankUp = m_tankObj.GetUp();
			D3DXVECTOR3 tankRight = m_tankObj.GetRight();

			D3DXVECTOR3 shellPos = tankPos + 3.5f * tankUp + 2.0f * tankLook;
			m_shells[i].object.SetPosition(shellPos);
			m_shells[i].object.SetRight(tankRight);
			m_shells[i].object.SetUp(tankUp);
			m_shells[i].object.SetLook(tankLook);

			m_shells[i].speed = 200.0f;

			m_shells[i].timer = 5.0f;

			//m_shells[i].vel = D3DXVECTOR3(tankLook * 10.0f);

			break;

		}
	}
}

void TankTraining::UpdateShells(float dt)
{
	for (int i = 0; i < MAX_SHELLS; i++)
	{
		if (m_shells[i].active)
		{
			
			m_shells[i].object.MoveOnZ(m_shells[i].speed * dt);

			m_shells[i].timer -= dt;
			if (m_shells[i].timer <= 0)
				m_shells[i].active = false;
		}
	}
}

void TankTraining::DrawShells()
{
	for (int i = 0; i < MAX_SHELLS; i++)
	{
		if (m_shells[i].active)
		{

			g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_shells[i].object.GetMatrix());
			m_shellMesh->DrawSubset(0);
		}
	}
}