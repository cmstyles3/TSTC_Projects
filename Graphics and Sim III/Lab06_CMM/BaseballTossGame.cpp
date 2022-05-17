#include "BaseballTossGame.h"

BaseballTossGameState::BaseballTossGameState() : State(BASEBALLTOSSGAME_STATE)
{
}

void BaseballTossGameState::Load()
{
	//D3DXCreateTextureFromFile(g_engine->GetDevice(), "buttons\\baseballtoss.bmp", &m_texture);
	m_currState = START;
	m_camera.SetPosition(0.0f, 0.0f, -100.0f);
	m_camera.SetLook(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_camera.SetRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	m_camera.SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	g_engine->GetDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	D3DXMatrixPerspectiveFovLH(&m_proj, D3DX_PI/4, 800.0f/600.0f, 1.0f, 1000.0f);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);

	VertexPD *vpd;
	D3DXCreateCylinder(g_engine->GetDevice(), 1.5f, 2.0f, 10.0f, 10, 1, &m_bottleMesh,0);
	m_bottleMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PD, g_engine->GetDevice(), &m_bottleMesh);
	m_bottleMesh->LockVertexBuffer(0, (void**)&vpd);
	for (int i = 0; i < m_bottleMesh->GetNumVertices(); i++)
	{
		vpd[i].color = D3DCOLOR_XRGB(255, 0, 0);
	}
	m_bottleMesh->UnlockVertexBuffer();

	m_bottles[0].pos = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
	m_bottles[1].pos = D3DXVECTOR3(-2.5f, -5.0f, 0.0f);
	m_bottles[2].pos = D3DXVECTOR3( 2.5f, -5.0f, 0.0f);

	m_font = new Font;
}

void BaseballTossGameState::Close()
{
	SAFE_DELETE(m_font);
	SAFE_RELEASE(m_bottleMesh);
	//SAFE_RELEASE(m_texture);
}

void BaseballTossGameState::OnResetDevice()
{
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);
}

void BaseballTossGameState::Update(float dt)
{
	Input *input = g_engine->GetInput();
	if (input->KeyDown(DIK_ESCAPE))
		g_engine->ChangeState(MENU_STATE);
	switch(m_currState)
	{
		case START:
			{
			}break;
		case PLAY:
			{

			}break;
		case RESULT:
			{
			}break;
	}

	D3DXMATRIX V;
	m_camera.GetViewMatrix(&V);
	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
}

void BaseballTossGameState::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();

	D3DXMATRIX I, R;
	D3DXMatrixIdentity(&I);
	D3DXMatrixRotationX(&R, D3DX_PI/2);
	for (int i = 0; i < NUM_BOTTLES; i++)
	{
		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, m_bottles[i].pos.x, m_bottles[i].pos.y, m_bottles[i].pos.z);
		g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &(R*T));
		m_bottleMesh->DrawSubset(0);
	}
	m_font->Draw(0,0,"UNDER CONTRUCTION\n\
					 Closed for repairs\n\
					 Will open next summer\n\n\n\n\n\n\
					 Press escape to go back to menu\n", 0xffffffff);
}