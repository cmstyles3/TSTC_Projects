#include "Sprite.h"



// Constructor
CSprite::CSprite()
{
	m_pSprite = 0;
	m_pTexture = 0;
	m_pDevice = 0;
	m_position.x = m_position.y = 0;
	m_center.x = m_center.y = 0;
	m_sourceRect.top = m_sourceRect.bottom = 0;
	m_sourceRect.left = m_sourceRect.left = 0;
	m_pFrames = 0;
	m_color = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
}

// Destructor
CSprite::~CSprite()
{
	Kill();
}


// Adds references for DIRECT3D DEVICE and D3DXSPRITE objects
// Also if numFrames is greater than zero, than it will allocate memory for frame info
int CSprite::Init(LPDIRECT3DDEVICE9 pDevice, LPD3DXSPRITE pSprite, int numFrames)
{
	SAFE_RELEASE(m_pDevice); // RELEASE Device

	m_pDevice = pDevice; // Add reference to Direct3D DEVICE Object
	m_pDevice->AddRef();
	
	SAFE_RELEASE(m_pSprite); // RELEASE Sprite

	m_pSprite = pSprite; // Add reference to Direct3D SPRITE Object
	m_pSprite->AddRef();
	
	// Allocate memory used to store frame info
	if (numFrames > 0)
	{
		m_numFrames = numFrames;
		m_pFrames = new Frame[numFrames];
	}

	return(1);
}


// Loads texture from file
int CSprite::Load(const char filename[])
{
	if (FAILED(D3DXCreateTextureFromFile(m_pDevice, filename, &m_pTexture)))
		return(0);

	return(1);
}

void CSprite::Kill()
{
	SAFE_DELETE_ARRAY(m_pFrames);
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pDevice);
}

void CSprite::Draw()
{
	m_pSprite->Draw(m_pTexture, &m_sourceRect, &m_center, &m_position, m_color);
}

void CSprite::Draw(int frameIndex)
{
	m_pSprite->Draw(m_pTexture, &m_pFrames[frameIndex].m_sourceRect, &m_center, &m_position, m_color);
}

void CSprite::Draw(int frameIndex, int x, int y)
{
	D3DXVECTOR3 position;
	position.x = x;
	position.y = y;
	position.z = 0;
	m_pSprite->Draw(m_pTexture, &m_pFrames[frameIndex].m_sourceRect, &m_center, &position, m_color);
}

void CSprite::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CSprite::SetCenter(float x, float y)
{
	m_center.x = x;
	m_center.y = y;
}

void CSprite::SetSourceRect(int left, int top, int right, int bottom)
{
	m_sourceRect.left = left;
	m_sourceRect.top = top;
	m_sourceRect.right = right;
	m_sourceRect.bottom = bottom;
}

void CSprite::SetColor(D3DCOLOR color)
{
	m_color = color;
}

void CSprite::SetFrame(int frameIndex, int x, int y, int width, int height)
{
	if (frameIndex < m_numFrames)
	{
		m_pFrames[frameIndex].m_sourceRect.left   = x;
		m_pFrames[frameIndex].m_sourceRect.top    = y;
		m_pFrames[frameIndex].m_sourceRect.right  = x + width;
		m_pFrames[frameIndex].m_sourceRect.bottom = y + height;
	}
}