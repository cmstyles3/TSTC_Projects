#include "D3DApp.h"
#include <vector>
#include <conio.h>

#define NUM_EXPLOSIONS 50
#define NUM_PARTICLES 50
#define NUM_BALLS 30

enum{FALLINGSTAR, BUTTERFLY, BUMBLEBEE, BOMB, EXPLOSION};


struct ARGB
{
	BYTE alpha, red, green, blue;
	ARGB()
	{
		alpha = 255;
		red = 255;
		green = 255;
		blue = 255;
	}
	ARGB(BYTE alpha, BYTE red, BYTE green, BYTE blue)
	{
		this->alpha = alpha;
		this->red = red;
		this->green = green;
		this->blue = blue;
	}
	ARGB operator=(ARGB rhs)
	{
		alpha = rhs.alpha;
		red = rhs.red;
		green = rhs.green;
		blue = rhs.blue;

		return *this;
	}
};

struct Projectile
{
	float x, y;
	float xv, yv;
	bool active;
	ARGB color;
	float angle;
	float timer;
};


struct Particle
{
	float x, y;
	float xv, yv;
	bool active;
	ARGB color;
	float life;
	float timer;

};


class MAIN : public D3DApp
{
	public:
		MAIN(HINSTANCE hInstance,  std::string caption, D3DDEVTYPE devType, DWORD requestedVP,
			int width = 800, int height = 600);
		~MAIN();

		void OnLostDevice();
		void OnResetDevice();
		void Update(float dt);
		void Draw(float dt);


		// balls functions
		void InitBalls();
		void StartBall();
		void UpdateBalls(float dt);
		void DrawBalls();

		// firework functions
		void StartParticles(int type, float x, float y, ARGB color);
		void UpdateParticles(float dt);
		void DrawParticles();

		void CheckParticleCollisions();

	private:
		LPD3DXSPRITE m_sprite;
		LPDIRECT3DTEXTURE9 m_tex, m_tex2;
		LPD3DXFONT m_font;
		float m_angle;

		// 

		int m_fireworkType;
		std::string m_strType[4];
		Projectile m_balls[NUM_BALLS];

		int m_numParticles;

		float m_gravity;
		float m_wind;
		float m_barrelSize;

		std::vector<Particle> m_particles;

		

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{

	MAIN app(hInstance, "Framework", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);

	g_d3dApp = &app;

	Sleep(200);
	return(g_d3dApp->run());
}

MAIN::MAIN(HINSTANCE hInstance,  std::string caption, D3DDEVTYPE devType, DWORD requestedVP,
		   int width, int height) : D3DApp(hInstance, caption, devType, requestedVP)
{
	m_sprite = 0;
	m_angle = 0;
	
	//if (GetDevice())
		if (FAILED(D3DXCreateTextureFromFile(GetDevice(), "cannon.bmp", &m_tex)))
			MessageBox(0, "MAIN", 0, 0);

		D3DXCreateTextureFromFile(GetDevice(), "projectile.dds", &m_tex2);



	D3DXFONT_DESC fontDesc;
	fontDesc.Height = 15;
	fontDesc.Width = 8;
	fontDesc.Weight = FW_BOLD;
	fontDesc.MipLevels = 0;
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(fontDesc.FaceName, "Times New Roman");
	D3DXCreateFontIndirect(GetDevice(), &fontDesc, &m_font);

	D3DXCreateSprite(GetDevice(), &m_sprite); 
	
	InitBalls();
	m_fireworkType = FALLINGSTAR;
	m_strType[0] = "FALLING STAR";
	m_strType[1] = "BUTTERFLY";
	m_strType[2] = "BUMBLEBEE";
	m_strType[3] = "BOMB";
	m_gravity = 60;
	m_wind = -0;
	m_barrelSize = 40;
	
	ShowCursor(false);
	
}

MAIN::~MAIN()
{
	m_tex->Release();
	m_tex2->Release();
	m_font->Release();
	m_sprite->Release();

}

void MAIN::OnLostDevice()
{
	m_font->OnLostDevice();
	m_sprite->OnLostDevice();

}

void MAIN::OnResetDevice()
{
	m_font->OnResetDevice();
	m_sprite->OnResetDevice();
}

void MAIN::Update(float dt)
{
	//PROCESS USER INPUT
	GetInput()->Update();

	if (GetInput()->KeyDown(DIK_ESCAPE))
		PostQuitMessage(0);

	static float shootTimer = 0;
	shootTimer -= dt;
	if (shootTimer <= 0) shootTimer = 0;
	if (GetInput()->KeyDown(DIK_SPACE) && shootTimer <= 0)
	{
		StartBall();
		shootTimer = .15f;
	} 
	if (GetInput()->KeyDown(DIK_LEFT))
	{
		m_angle -= dt * 0.5f;
		if (m_angle <= (-D3DX_PI/4)) m_angle = -D3DX_PI/4;
	}
	if (GetInput()->KeyDown(DIK_RIGHT))
	{
		m_angle += dt * 0.5f;
		if (m_angle >= (D3DX_PI/4)) m_angle = D3DX_PI/4;
	}
	if (GetInput()->KeyDown(DIK_UP))
	{
		m_wind -= dt * 1.5f;
	}
	if (GetInput()->KeyDown(DIK_DOWN))
	{
		m_wind += dt * 1.5f;
	}

	if (GetInput()->KeyDown(DIK_S))
	{
		m_fireworkType = FALLINGSTAR;
	}

	if (GetInput()->KeyDown(DIK_B))
	{
		m_fireworkType = BUMBLEBEE;
	}
	if (GetInput()->KeyDown(DIK_F))
	{
		m_fireworkType = BUTTERFLY;
	}
	if (GetInput()->KeyDown(DIK_1))
	{
		m_fireworkType = BOMB;
	}
	if (GetInput()->KeyDown(DIK_SUBTRACT))
	{
		m_barrelSize -= dt * 1.5f;
		if (m_barrelSize < 40) m_barrelSize = 40;
	}
	if (GetInput()->KeyDown(DIK_ADD))
	{
		m_barrelSize += dt * 1.5f;
	}

	m_numParticles = m_particles.capacity();
	//UPDATE OBJECTS
	UpdateBalls(dt);
	UpdateParticles(dt);
	CheckParticleCollisions();
}

void MAIN::Draw(float dt)
{
	GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);//D3DCOLOR_XRGB(rand()%256,rand()%256,rand()%256), 1.0f, 0);
	GetDevice()->BeginScene();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

	// DRAW CANNON
	RECT srcRect = { 0, 0, 32, 32};
	D3DXMATRIX S, R, T;
	D3DXVECTOR3 center((float)800/2.0f, (float)600/2.0f, 0);
	D3DXMatrixRotationZ(&R, m_angle);
	D3DXMatrixScaling(&(S), m_barrelSize/40, 1.0f, 1.0f);
	D3DXMatrixTranslation(&T, 400, 600, 0);
	m_sprite->SetTransform(&(S * R * T));
	m_sprite->Draw(m_tex, &srcRect, &D3DXVECTOR3(16,32,0), &D3DXVECTOR3(0,0,0), 0xffffffff); 
	
	DrawBalls();
	DrawParticles();


	m_sprite->End();

	RECT rect = { 0, 0, 800, 600 };
	char buffer[150];
	sprintf(buffer, "Angle = %i,\nFirework type = %s, \nWind = %.2f, \nBarrel Size = %.2f \nNumber of Particles = %i",
		(int)(m_angle * 180/D3DX_PI), m_strType[m_fireworkType].c_str(), m_wind, m_barrelSize, m_numParticles);
	m_font->DrawText(0, buffer, -1, &rect, DT_LEFT, 0xffffffff);

	GetDevice()->EndScene();
	GetDevice()->Present(0, 0, 0, 0);
}

void MAIN::InitBalls()
{
	ZeroMemory(&m_balls, sizeof(Projectile)*NUM_BALLS);
}

void MAIN::StartBall()
{
	for (int i = 0; i < NUM_BALLS; i++)
	{
		if (!m_balls[i].active)
		{
			m_balls[i].timer = (rand()%1000) * 0.001 + 1;
			m_balls[i].angle = m_angle;
			m_balls[i].x = 400 + cosf(m_angle-D3DX_PI*.5) * 32;
			m_balls[i].y = 599 + sinf(m_angle-D3DX_PI*.5) * 32;
			
			m_balls[i].xv = cosf(m_angle-D3DX_PI*.5) * 250 * 40/m_barrelSize;
			m_balls[i].yv = sinf(m_angle-D3DX_PI*.5) * 350 * 40/m_barrelSize;
			m_balls[i].active = true;
	
			m_balls[i].color.red = rand()%256;
			m_balls[i].color.green = rand()%256;
			m_balls[i].color.blue = rand()%256; // = D3DCOLOR_XRGB(rand()%256, rand()%256, rand()%256);
			m_balls[i].color.alpha = 255;
			break;
		}
	}
}

void MAIN::UpdateBalls(float dt)
{
	for (int i = 0; i < NUM_BALLS; i++)
	{
		if (m_balls[i].active)
		{
			
			m_balls[i].timer -= dt;
			if (m_balls[i].timer <= 0) 
			{
				m_balls[i].active = false;
				StartParticles(m_fireworkType, m_balls[i].x, m_balls[i].y, m_balls[i].color);

			}
			for (int j = i + 1; j < NUM_BALLS; j++)
			{
				if (m_balls[j].active)
				{
					if (abs(m_balls[i].x - m_balls[j].x) < 10 &&
						abs(m_balls[i].y - m_balls[j].y) < 10)
					{
						m_balls[i].active = false;
						m_balls[j].active = false;
						StartParticles(m_fireworkType, m_balls[i].x, m_balls[i].y, m_balls[i].color);
						StartParticles(m_fireworkType, m_balls[j].x, m_balls[j].y, m_balls[j].color);
					}
				}
			}
			m_balls[i].x += m_balls[i].xv * dt;
			m_balls[i].y += m_balls[i].yv * dt;

			m_balls[i].xv += m_wind * dt;
			m_balls[i].yv += m_gravity * dt;
			

			if (m_balls[i].x < 0 || m_balls[i].x >= 800 ||
				m_balls[i].y < 0 || m_balls[i].y >= 600)
				m_balls[i].active = false;
		}
	}
}

void MAIN::DrawBalls()
{
	
	for (int i = 0; i < NUM_BALLS; i++)
	{
		if (m_balls[i].active)
		{
			RECT srcRect = { 0, 0, 32, 32};
			D3DXMATRIX R, T;
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			m_sprite->SetTransform(&(I));

			m_sprite->Draw(m_tex2, 0, &D3DXVECTOR3(16,16,0), &D3DXVECTOR3(m_balls[i].x, m_balls[i].y, 0),  
				D3DCOLOR_XRGB(m_balls[i].color.red, m_balls[i].color.green, m_balls[i].color.blue)); 
		}
	}
}

void MAIN::StartParticles(int type, float x, float y, ARGB color)
{
	int distance = 0;

	switch(type)
	{
		case FALLINGSTAR:
	
			for (int i = 0; i < NUM_PARTICLES; i++)
			{
				Particle p;
				p.active = true;
				p.color = color;
				p.timer  = 3;
				p.life = 255;
				p.x = x;
				p.y = y;
				float dir = D3DX_PI * 2 * i / NUM_PARTICLES;
				p.xv = cosf(dir) * (rand()%200+2);
				p.yv = sinf(dir) * (rand()%200+2);
				m_particles.push_back(p);
			}
			break;

		case BUTTERFLY:

			for (int i = 0; i < 20; i++)
			{
				Particle p;
				p.active = true;
				p.color = color;
				p.timer  = 3;
				p.life = 255;
				p.x = x;
				p.y = y;
				float dir = -D3DX_PI/4;
				p.xv = cosf(dir) * (rand()%200+2);
				p.yv = sinf(dir) * (rand()%200+2);
				m_particles.push_back(p);
			}

			for (int i = 0; i < 20; i++)
			{
				Particle p;
				p.active = true;
				p.color = color;
				p.timer  = 3;
				p.life = 255;
				p.x = x;
				p.y = y;
				float dir = D3DX_PI+D3DX_PI/4;
				p.xv = cosf(dir) * (rand()%200+2);
				p.yv = sinf(dir) * (rand()%200+2);
				m_particles.push_back(p);
			}

			break;

		case BUMBLEBEE:

			{
			int randNum = rand()%2;
			
			if(randNum==0)
			{
				for (int i = 0; i < 30; i++)
				{
					
					Particle p;
					p.active = true;
					p.color = color;
					p.timer  = 3;
					p.life = 255;
					p.x = x;
					p.y = y;
				
					float dir = D3DX_PI * 2 * (i+1) / 15;
					p.xv = cosf(dir) * distance + (rand()%2+1);
					p.yv = sinf(dir) * distance + (rand()%2+1);
					distance += 3;
					m_particles.push_back(p);
				}
			}
			else
			{
				
				for (int i = 0; i < 30; i++)
				{
					
					Particle p;
					p.active = true;
					p.color = color;
					p.timer  = 3;
					p.life = 255;
					p.x = x;
					p.y = y;
				
					float dir = -D3DX_PI * 2 * (i+1) / 15;
					p.xv = cosf(dir) * distance + (rand()%2+1);
					p.yv = sinf(dir) * distance + (rand()%2+1);
					distance += 3;
					m_particles.push_back(p);
				}
			
			}
			
			}break;

		case BOMB:
			for (int i = 0; i < 10; i++)
				{
					
					Particle p;
					p.active = true;
					p.color = color;
					p.timer  = 3;
					p.life = 255;
					p.x = x;
					p.y = y;
				
					
					p.xv = 0;
					p.yv = -100;
					distance += 3;
					m_particles.push_back(p);
				}
			break;

		case EXPLOSION:
			for (int i = 0; i < 10; i++)
			{
				Particle p;
				p.active = true;
				ARGB color(255, rand()%256, rand()%256, rand()%256);
				p.color = color;
				p.timer  = 3;
				p.life = 255;
				p.x = x;
				p.y = y;
				float dir = D3DX_PI * 2 * i / 10;
				p.xv = cosf(dir) * (rand()%50+10);
				p.yv = sinf(dir) * (rand()%50+10);
				m_particles.push_back(p);
			}
			break;
	}

	
}

void MAIN::UpdateParticles(float dt)
{

	for (int i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].active)
		{
			m_particles[i].x += m_particles[i].xv * dt;
			m_particles[i].y += m_particles[i].yv * dt;

			m_particles[i].xv += m_wind * dt;
			m_particles[i].yv += m_gravity * dt;

			m_particles[i].timer -= dt;
			m_particles[i].life -= dt *2;

			if (m_particles[i].x < -20  || m_particles[i].x >= 820 ||
				m_particles[i].y < -20  || m_particles[i].y >= 620 || 
				m_particles[i].life <= 0)
			{
				m_particles[i].active = false;
				memcpy(&m_particles[i], &m_particles[m_particles.size() - 1], sizeof(Particle));
				//m_particles[i] = m_particles[m_particles.size() - 1];
				
				m_particles.pop_back();
				std::vector<Particle>(m_particles).swap(m_particles);
			}
		}
	}
}

void MAIN::DrawParticles()
{

	for (int i = 0; i < m_particles.size(); i++)
	{
		if (m_particles[i].active)
		{
			RECT rect = {15, 15, 16, 16};
			D3DXMATRIX I;
			D3DXMatrixIdentity(&I);
			
			m_sprite->SetTransform(&I);
			//D3DCOLOR *color = &curr->color;
			
			m_sprite->Draw(m_tex2, &rect, &D3DXVECTOR3(16, 16, 0), &D3DXVECTOR3(m_particles[i].x, m_particles[i].y, 0), 
				D3DCOLOR_ARGB((int)m_particles[i].life, (int)m_particles[i].color.red, (int)m_particles[i].color.green, (int)m_particles[i].color.blue));
		}
	}
	
}

void MAIN::CheckParticleCollisions()
{
	
	for (int i = 0; i < m_particles.size(); i++)
	{
		for (int j = i + 1; j < m_particles.size(); j++)
		{
			if (abs(m_particles[i].x - m_particles[j].x) < 0.01f &&
				abs(m_particles[j].y - m_particles[j].y) < 0.01f)
			{
				if (m_particles[i].timer < 1 && m_particles[j].timer < 1)
				{
					StartParticles(EXPLOSION, m_particles[i].x-1, m_particles[i].y, m_particles[i].color);
					StartParticles(EXPLOSION, m_particles[j].x+1, m_particles[j].y, m_particles[j].color);

					memcpy(&m_particles[i], &m_particles[m_particles.size() - 1], sizeof(Particle));
					m_particles.pop_back();

					memcpy(&m_particles[j], &m_particles[m_particles.size() - 1], sizeof(Particle));
					
					m_particles.pop_back();
					
				}

			}

		}
	}
	
}