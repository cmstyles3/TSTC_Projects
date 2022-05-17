// LOADING.CPP
// DESCRIPTION: Used for displaying and updating the loading screen

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Main.h"

Loading::Loading() : State(LOADING_STATE)
{

}

void Loading::Load()
{

	m_ready = false;
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets\\Textures\\LoadingScreen.bmp", &m_loadingTexture);

	m_currentFrame = 0;
	m_loadingAnim = 0;

	m_frames[0].left = 800;
	m_frames[0].top = 0;
	m_frames[0].right = m_frames[0].left + 75;
	m_frames[0].bottom = m_frames[0].top + 75;

	m_frames[1].left = 800;
	m_frames[1].top = 75;
	m_frames[1].right = m_frames[1].left + 75;
	m_frames[1].bottom = m_frames[1].top + 75;

	m_frames[2].left = 800;
	m_frames[2].top = 150;
	m_frames[2].right = m_frames[2].left + 75;
	m_frames[2].bottom = m_frames[2].top + 75;

	m_frames[3].left = 800;
	m_frames[3].top = 225;
	m_frames[3].right = m_frames[3].left + 75;
	m_frames[3].bottom = m_frames[3].top + 75;

	m_frames[4].left = 800;
	m_frames[4].top = 300;
	m_frames[4].right = m_frames[4].left + 75;
	m_frames[4].bottom = m_frames[4].top + 75;

	m_frames[5].left = 800;
	m_frames[5].top = 375;
	m_frames[5].right = m_frames[5].left + 75;
	m_frames[5].bottom = m_frames[5].top + 75;

	m_frames[6].left = 800;
	m_frames[6].top = 450;
	m_frames[6].right = m_frames[6].left + 75;
	m_frames[6].bottom = m_frames[6].top + 75;

	m_frames[7].left = 800;
	m_frames[7].top = 525;
	m_frames[7].right = m_frames[7].left + 75;
	m_frames[7].bottom = m_frames[7].top + 75;

	m_frameCounter = 0.0f;

	D3DXCreateLine(g_engine->GetDevice(), &m_line);
	m_vertexlist[0] = D3DXVECTOR2(144, 484);
	m_vertexlist[1] = D3DXVECTOR2(656, 484);
	m_vertexlist[2] = D3DXVECTOR2(656, 516);
	m_vertexlist[3] = D3DXVECTOR2(144, 516);
	m_vertexlist[4] = D3DXVECTOR2(144, 484);
}

void Loading::Close()
{
	SAFE_RELEASE(m_line);
	SAFE_RELEASE(m_loadingTexture);
}

void Loading::OnLostDevice()
{
	m_line->OnLostDevice();
}

void Loading::OnResetDevice()
{
	m_line->OnResetDevice();
}

void Loading::Update(float dt)
{
	if (m_ready)
	{
		if (g_engine->GetInput()->KeyDown(DIK_SPACE))
			g_engine->ChangeState(MAINMENU_STATE);
		if (g_engine->GetInput()->KeyDown(DIK_ESCAPE))
			PostQuitMessage(0);

		static DWORD timer = timeGetTime();
		if (timer < timeGetTime() - 500)
		{
			flash = !flash;
			timer = timeGetTime();
		}
	}

	if (!m_ready)
	{
		m_frameCounter += dt;
		if (m_frameCounter >= 0.3f)
		{
			m_currentFrame++;
			if (m_currentFrame > 7)
				m_currentFrame = 0;
			m_frameCounter = 0;
		}

		static DWORD loading = timeGetTime();
		if (loading < timeGetTime() - 500)
		{
			m_loadingAnim ++;
			if (m_loadingAnim > 3) m_loadingAnim = 0;
			loading = timeGetTime();
		}
	}

	

	
}

void Loading::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();

	sprite->Begin(0);
	RECT back = {0, 0, 800, 600};
	sprite->Draw(m_loadingTexture, &back, 0, 0, 0xffffffff);

	RECT rect = {0, 600, percentLoaded * 512, 632};
	D3DXVECTOR3 position(400, 500, 0);
	D3DXVECTOR3 center(256, 16, 0);
	sprite->Draw(m_loadingTexture, &rect, &center, &position, 0xffffffff);

	if (!m_ready)
	{
		position = D3DXVECTOR3(720, 0, 0);
		sprite->Draw(m_loadingTexture, &m_frames[m_currentFrame], 0, &position, 0xffffffff);
	}

	if (m_ready)
	{
		RECT rect = {0, 632, 512, 664};
		sprite->Draw(m_loadingTexture, &rect, 0, &D3DXVECTOR3(190, 540, 0), flash ? 0xffffffff : 0xff777777);
	}

	sprite->End();

	m_line->SetWidth(4.0f);
	m_line->Begin();
	
	m_line->Draw(m_vertexlist, 5, 0xffff0000);
	m_line->End();

	if (!m_ready)
	{
		if (m_loadingAnim == 0)
			g_smallFont.Draw(0, 0, 0xffffffff, "LOADING");
		if (m_loadingAnim == 1)
			g_smallFont.Draw(0, 0, 0xffffffff, "LOADING.");
		if (m_loadingAnim == 2)
			g_smallFont.Draw(0, 0, 0xffffffff, "LOADING..");
		if (m_loadingAnim == 3)
			g_smallFont.Draw(0, 0, 0xffffffff, "LOADING...");
	}
}

void Loading::SetReady(bool ready)
{
	m_ready = ready;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////