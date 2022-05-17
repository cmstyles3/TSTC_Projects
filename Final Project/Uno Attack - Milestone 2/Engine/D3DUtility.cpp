// D3DUTILITY.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine.h"


D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.2f;
	light.Diffuse   = *color * 0.8f;
	light.Specular  = *color * 0.2f;
	light.Direction = *direction;

	return light;
}

void SetFog(float start, float end, D3DCOLOR color)
{
	g_engine->GetDevice()->SetRenderState(D3DRS_FOGENABLE, true);
 
    // Set the fog color.
	g_engine->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, color);
	
    
    // Set fog parameters.
	//if( Mode == D3DFOG_LINEAR )
    {
        g_engine->GetDevice()->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        g_engine->GetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&start));
        g_engine->GetDevice()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&end));
    }

}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////