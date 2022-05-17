// MAIN.CPP

#include "Main.h"

// G L O B A L S ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
Font *g_font = NULL;
LPDIRECT3DTEXTURE9 g_wallTexture;

Camera g_camera;

POLYGON	*g_polygonList;	    
NODE	*g_BSPTreeRootNode; 


BYTE BSPMAP[] ={0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				0,0,2,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
				0,2,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,
				1,0,0,0,0,0,3,1,0,0,0,0,0,1,1,1,1,1,0,1,
				0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,
				0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,
				0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,3,1,
				0,2,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,
				0,1,0,0,0,0,1,2,0,0,0,1,0,0,0,1,0,0,0,1,
				0,1,0,0,0,1,2,0,0,0,0,1,1,0,0,0,0,0,0,1,
				0,1,0,0,0,1,0,0,0,0,0,3,1,0,0,0,0,0,0,1,
				0,1,0,1,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
				1,2,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,
				1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,1,1,1,1,0,1,0,0,0,0,0,1,1,1,1,1,1,1,1,
				0,0,0,1,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,
				0,0,2,0,0,0,1,0,0,1,0,3,0,0,0,0,0,0,0,0,
				0,2,0,0,0,0,1,0,0,1,0,0,3,0,0,0,0,0,0,0,
				1,0,0,0,0,0,1,0,0,1,0,0,0,1,1,1,1,1,0,1,
				0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
				0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,
				0,1,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,3,1,
				0,2,0,0,0,0,0,1,0,1,1,1,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,1,0,0,3,1,0,0,0,1,0,0,0,1,
				0,1,0,0,0,0,1,2,0,0,0,1,0,0,0,1,0,0,0,1,
				0,1,0,0,0,1,2,0,0,0,0,1,1,0,0,0,0,0,0,1,
				0,1,0,0,0,1,0,0,0,0,0,3,1,0,0,0,0,0,0,1,
				0,1,0,1,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
				1,2,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,
				1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,1,2,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
				1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	EngineSetup setup;
	setup.instance = instance;
	setup.windowed = true;
	setup.name = "LAB08 - Solid-leaf BSP TREE";
	setup.Init = Init;
	setup.Kill = Kill;
	setup.OnLostDevice = OnLostDevice;
	setup.OnResetDevice = OnResetDevice;
	setup.Update = Update;
	setup.Draw = Draw;
	setup.clearColor = D3DCOLOR_XRGB(70, 100, 150);

	new Engine(&setup);
	g_engine->Run();

	return(1);
}

void Init()
{
	g_font = new Font;

	SetD3DOptions();

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "wall.bmp", &g_wallTexture);
		
	InitLevelPolys();
	
	g_BSPTreeRootNode = new NODE;
	BuildBspTree(g_BSPTreeRootNode, g_polygonList);

}

void Kill()
{
	DeleteBSPTree(g_BSPTreeRootNode);

	SAFE_DELETE(g_font);
	SAFE_RELEASE(g_wallTexture);
}

void OnLostDevice()
{
	g_font->OnLostDevice();
}

void OnResetDevice()
{
	g_font->OnResetDevice();
	SetD3DOptions();
}

void Update(float dt)
{
	float x, y, z;
	g_camera.GetPosition(x, y, z);
	D3DXVECTOR3 cameraPos(x, y, z);
	D3DXVECTOR3 oldCameraPos = cameraPos;
	Input *input = g_engine->GetInput();
	if (input->KeyDown(DIK_ESCAPE))
		PostQuitMessage(0);

	if (input->KeyDown(DIK_A))
		g_camera.Strafe(-3.0f * dt);
	if (input->KeyDown(DIK_D))
		g_camera.Strafe(3.0f * dt);
	if (input->KeyDown(DIK_W))
		g_camera.Walk(6.0f * dt);
	if (input->KeyDown(DIK_S))
		g_camera.Walk(-6.0f * dt);
	if (input->KeyDown(DIK_LEFT))
		g_camera.Yaw(-2.0f * dt);
	if (input->KeyDown(DIK_RIGHT))
		g_camera.Yaw(2.0f * dt);


	g_camera.GetPosition(x, y, z);
	cameraPos = D3DXVECTOR3(x, y, z);
	D3DXVECTOR3 pos1(x-0.2, y, z);
	D3DXVECTOR3 pos2(x+0.2, y, z);
	D3DXVECTOR3 pos3(x, y, z-0.2);
	D3DXVECTOR3 pos4(x, y, z+0.2);
	D3DXVECTOR3 pos5(x-0.2, y, z-0.2);
	D3DXVECTOR3 pos6(x+0.2, y, z-0.2);
	D3DXVECTOR3 pos7(x-0.2, y, z+0.2);
	D3DXVECTOR3 pos8(x+0.2, y, z+0.2);
	if (LineOfSight(&pos1, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos2, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos3, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos4, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos5, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos6, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos7, &oldCameraPos, g_BSPTreeRootNode) == false ||
		LineOfSight(&pos8, &oldCameraPos, g_BSPTreeRootNode) == false)
	{
		g_camera.SetPosition(oldCameraPos.x,
			oldCameraPos.y, oldCameraPos.z);
	}

	D3DXMATRIX V;
	g_camera.GetViewMatrix(&V);
	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
}

void Draw()
{
	float x, y, z;
	g_camera.GetPosition(x, y, z);
	D3DXVECTOR3 pos(x, y, z);
	y = pos.y = 1.5f;
	g_camera.SetPosition(x, y, z);
	
	WalkBspTree(g_BSPTreeRootNode, (D3DXVECTOR3*)&pos); 

	g_font->Draw(0, 0, "CONTROLS\n\
					Press 'W' to move forward\n\
					Press 'S' to move backward\n\
					Press 'A' to strafe to the left\n\
					Press 'D' to strafe to the right\n\
					Press left arrow to turn left\n\
					Press right arrow to turn right\n", 0xffffffff);
}

void SetD3DOptions()
{
	// SET SAMPLING FILTER FOR TEXTURES TO LINEAR
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*.25f, 800.0f/600.0f, 0.1f, 1000.0f);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &proj);

	g_engine->GetDevice()->SetFVF(D3DFVF_PDT);
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
}

void BuildBspTree(NODE *CurrentNode, POLYGON *PolyList)
{
	POLYGON *polyTest	 = NULL;
	POLYGON *FrontList	 = NULL;
	POLYGON *BackList	 = NULL;
	POLYGON *NextPolygon = NULL;
	POLYGON *FrontSplit	 = NULL;
	POLYGON *BackSplit	 = NULL;

	
	CurrentNode->Splitter = SelectBestSplitter(PolyList);
	
	polyTest = PolyList;

	
	while (polyTest != NULL) 
    {
		
		NextPolygon = polyTest->Next;

		if (polyTest != CurrentNode->Splitter) 
        {
			switch (ClassifyPoly(CurrentNode->Splitter, polyTest)) 
            {
				case IN_FRONT_OF_PLANE:
					polyTest->Next = FrontList;
					FrontList	   = polyTest;		
					break;

				case BEHIND_PLANE:
					polyTest->Next = BackList;
					BackList	   = polyTest;	
					break;

				case STRADDLING_PLANE:
					
					FrontSplit = new POLYGON;
					BackSplit  = new POLYGON;
					ZeroMemory(FrontSplit,sizeof(POLYGON));
					ZeroMemory(BackSplit,sizeof(POLYGON));
					
					SplitPolygon(polyTest, CurrentNode->Splitter, FrontSplit, BackSplit);
					
					delete polyTest;
					
					FrontSplit->Next = FrontList;
					FrontList		 = FrontSplit;
					BackSplit->Next	 = BackList;
					BackList		 = BackSplit;
					break;

				default:
					break;
			}
		}
		polyTest = NextPolygon;
    }

	if (FrontList == NULL) 
    {
		NODE *leafnode		= new NODE;
		ZeroMemory(leafnode,sizeof(leafnode));
		leafnode->isLeaf	= true;
		leafnode->isSolid	= false;	
		leafnode->Front		= NULL;
		leafnode->Back		= NULL;
		leafnode->Splitter	= NULL;
		CurrentNode->Front	= leafnode;
	} 
    else 
    {
		NODE *newnode		= new NODE;
		ZeroMemory(newnode,sizeof(newnode));
		newnode->isLeaf	= false;
		CurrentNode->Front	= newnode;
		BuildBspTree(newnode,FrontList);
	} 

	if (BackList == NULL) 
    {
		NODE *leafnode      = new NODE;
		ZeroMemory(leafnode,sizeof(leafnode));
		leafnode->isLeaf   = true;
		leafnode->isSolid  = true;
		leafnode->Front     = NULL;
		leafnode->Back      = NULL;
		leafnode->Splitter  = NULL;
		CurrentNode->Back   = leafnode;;
	}
    else 
    {
		NODE *newnode     = new NODE;
		ZeroMemory(newnode,sizeof(newnode));
		newnode->isLeaf  = false;
		CurrentNode->Back = newnode;
		BuildBspTree(newnode,BackList);
	}
}

int ClassifyPoly(POLYGON *Plane, POLYGON *Poly)
{
	D3DXVECTOR3 *vec1 = (D3DXVECTOR3*)&Plane->VertexList[0];
	int	Infront = 0;
    int Behind  = 0;
    int OnPlane = 0;
	float result;
	
	
	for (int i = 0; i < Poly->numVertices; i++) 
    {
		D3DXVECTOR3 *vec2	  = (D3DXVECTOR3 *)&Poly->VertexList[i];
		D3DXVECTOR3 Direction = (*vec1) - (*vec2);
		
		result = D3DXVec3Dot(&Direction,&Plane->Normal);
		
		if (result > 0.001f) 
			Behind++;
        else if (result < -0.001f) 
			Infront++; 
        else 
        {
			OnPlane++;
			Infront++;
			Behind++;
		}
	}

	if (OnPlane == Poly->numVertices)
        return IN_FRONT_OF_PLANE;

	if (Behind  == Poly->numVertices)
        return BEHIND_PLANE;

	if (Infront == Poly->numVertices)
        return IN_FRONT_OF_PLANE;

	return STRADDLING_PLANE;
}

int ClassifyPoint(D3DXVECTOR3 *pos, POLYGON *Plane)
{
	D3DXVECTOR3 *vec1	  = (D3DXVECTOR3 *)&Plane->VertexList[0];
	D3DXVECTOR3 Direction = (*vec1) - (*pos);
	float		result	  = D3DXVec3Dot(&Direction,&Plane->Normal);

	if (result < -0.001f)
        return IN_FRONT_OF_PLANE;

	if (result >  0.001f)
        return BEHIND_PLANE;

	return COPLANAR_WITH_PLANE;
}

void WalkBspTree(NODE *Node, D3DXVECTOR3 *pos)
{
	if (Node->isLeaf == true)
        return;

	int result = ClassifyPoint(pos, Node->Splitter);
	
	if (result == IN_FRONT_OF_PLANE)
    {
		if (Node->Back  != NULL) 
            WalkBspTree(Node->Back, pos);

		g_engine->GetDevice()->SetTexture(0, g_wallTexture);
		g_engine->GetDevice()->SetFVF(D3DFVF_VERTEX);
		g_engine->GetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
											  0,
											  Node->Splitter->numVertices,
											  Node->Splitter->numIndices / 3,
											  &Node->Splitter->Indices[0],
											  D3DFMT_INDEX16,
											  &Node->Splitter->VertexList[0],
											  sizeof(VERTEX));

		if (Node->Front != NULL)
            WalkBspTree(Node->Front, pos);

		return;
	} 

	
	if (Node->Front != NULL) 
        WalkBspTree(Node->Front, pos);

	if (Node->Back != NULL) 
        WalkBspTree(Node->Back,  pos);

	return;
}

void InitLevelPolys()
{
	VERTEX	 vertList[4][4];
	POLYGON	*child = NULL;
	int		 direction[4];

	g_polygonList = NULL;

	for (int y = 0; y < 40; y++)
    {
		for (int x = 0; x < 20; x++)
        {
			ZeroMemory(direction, sizeof(int) * 4);
			
			int offset = (y * 20) + x;
						
			if (BSPMAP[offset] != 0) 
            {
				if (BSPMAP[offset] == 2) 
                {	
					vertList[0][0] = SetVertex(x-10.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
					vertList[0][1] = SetVertex(x- 9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
					vertList[0][2] = SetVertex(x- 9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
					vertList[0][3] = SetVertex(x-10.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
					direction[0]   = 1;
					
					if (x > 0) 
                    {
						if (BSPMAP[offset-1] == 0) 
                        {
							vertList[1][0] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[1][1] = SetVertex(x-10.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[1][2] = SetVertex(x-10.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[1][3] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[1]   = 1;
						}
					}

					if (y > 0) 
                    {
						if (BSPMAP[offset-20] == 0) 
                        {
							vertList[2][0] = SetVertex(x- 9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[2][1] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[2][2] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[2][3] = SetVertex(x- 9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[2]   = 1;
						}
					}
				}
				
				if (BSPMAP[offset] == 3)
                {	
					vertList[0][0] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
					vertList[0][1] = SetVertex(x- 9.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
					vertList[0][2] = SetVertex(x- 9.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
					vertList[0][3] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
					direction[0]   = 1;
	
					if(x < 19)
                    {
						if (BSPMAP[offset+1] == 0)
                        {
							vertList[1][0] = SetVertex(x-9.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[1][1] = SetVertex(x-9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[1][2] = SetVertex(x-9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[1][3] = SetVertex(x-9.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[1]   = 1;
						}
					}

					if (y > 0)
                    {
						if (BSPMAP[offset-20] == 0)
                        {
							vertList[2][0] = SetVertex(x- 9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[2][1] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[2][2] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[2][3] = SetVertex(x- 9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[2]   = 1;
						}
					}
				}				
				
				if (BSPMAP[offset] == 1)
                {
					if (x > 0)
                    {
						if (BSPMAP[offset-1] == 0)
                        {
							vertList[0][0] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[0][1] = SetVertex(x-10.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[0][2] = SetVertex(x-10.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[0][3] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[0]   = 1;
						}
					}
					
					if (x < 19) 
                    {
						if (BSPMAP[offset+1] == 0) 
                        {
							vertList[1][0] = SetVertex(x-9.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[1][1] = SetVertex(x-9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[1][2] = SetVertex(x-9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[1][3] = SetVertex(x-9.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[1]   = 1;
						}
					}
	
					if (y > 0) 
                    {
						if (BSPMAP[offset-20] == 0) 
                        {
							vertList[2][0] = SetVertex(x- 9.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[2][1] = SetVertex(x-10.5f,3.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[2][2] = SetVertex(x-10.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[2][3] = SetVertex(x- 9.5f,0.0f,(20.0f-y)+0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[2]   = 1;
						}
					}

					if (y < 39)
                    {	
						if (BSPMAP[offset+20] == 0)
                        {
							vertList[3][0] = SetVertex(x-10.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,0.0f);
							vertList[3][1] = SetVertex(x- 9.5f,3.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,0.0f);
							vertList[3][2] = SetVertex(x- 9.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f);
							vertList[3][3] = SetVertex(x-10.5f,0.0f,(20.0f-y)-0.5f, 1.0f,1.0f,1.0f,1.0f, 0.0f,1.0f);
							direction[3]   = 1;
						}
					}

				}

				for (int a = 0; a < 4; ++a)
                {
					if (direction[a] != 0)
                    {
						if (g_polygonList == NULL)
                        {
							g_polygonList = AddPolygon(NULL,&vertList[a][0],4);
							child = g_polygonList;
						}
                        else
                        {
							child = AddPolygon(child,&vertList[a][0],4);
						}
					}
				}
			}
		}
	}
}

bool GetIntersect(D3DXVECTOR3 *linestart, D3DXVECTOR3 *lineend, 
				   D3DXVECTOR3 *vertex, D3DXVECTOR3 *normal, 
				   D3DXVECTOR3 *intersection, float *percentage)
{
	D3DXVECTOR3 direction;
    D3DXVECTOR3 L1;
	float		linelength;
    float       dist_from_plane;

	direction.x = lineend->x - linestart->x;
	direction.y = lineend->y - linestart->y;
	direction.z = lineend->z - linestart->z;

	linelength = D3DXVec3Dot(&direction, normal);
	
	if(fabsf(linelength) < 0.001f) 
        return false;

	L1.x = vertex->x - linestart->x;
	L1.y = vertex->y - linestart->y;
	L1.z = vertex->z - linestart->z;

	dist_from_plane = D3DXVec3Dot(&L1, normal);

	*percentage	= dist_from_plane / linelength; 

	if (*percentage < 0.0f || *percentage > 1.0f) 
        return false;

	intersection->x = linestart->x + direction.x * (*percentage);
	intersection->y = linestart->y + direction.y * (*percentage);
	intersection->z = linestart->z + direction.z * (*percentage);	
	return true;
}

void SplitPolygon(POLYGON *Poly, POLYGON *Plane, 
				   POLYGON *FrontSplit, POLYGON *BackSplit)
{
	VERTEX FrontList[50];
	VERTEX BackList[50];
    int	PointLocation[50];
    int CurrentVertex = 0;
	int	FrontCounter  = 0;
	int	BackCounter   = 0;
	int	InFront       = 0;
    int Behind        = 0;
    int OnPlane       = 0;
    int Location      = 0;

	int i;
	for (i = 0; i < Poly->numVertices; i++)	
    {
		Location = ClassifyPoint((D3DXVECTOR3*)&Poly->VertexList[i], Plane);

		if (Location == IN_FRONT_OF_PLANE)
			++InFront;
		else if (Location == BEHIND_PLANE)
			++Behind;
		else
			++OnPlane;

		PointLocation[i] = Location;
	}
	

	if (!InFront) 
    {
		memcpy(BackList, Poly->VertexList, Poly->numVertices * sizeof(VERTEX));
		BackCounter = Poly->numVertices;
	}

	if (!Behind) 
    {
		memcpy(FrontList, Poly->VertexList, Poly->numVertices * sizeof(VERTEX));
		FrontCounter = Poly->numVertices;
	}

	if (InFront && Behind) 
    {
		for (i = 0; i < Poly->numVertices; i++) 
        {
			
			CurrentVertex = (i+1) % Poly->numVertices;

			if (PointLocation[i] == COPLANAR_WITH_PLANE) 
            {
				FrontList[FrontCounter] = Poly->VertexList[i];
				FrontCounter++;
				BackList[BackCounter] = Poly->VertexList[i];
				BackCounter++;
				continue; 
			}
			if (PointLocation[i] == IN_FRONT_OF_PLANE) 
            {
				FrontList[FrontCounter] = Poly->VertexList[i];
				FrontCounter++;
			} 
            else 
            {
				BackList[BackCounter] = Poly->VertexList[i];
				BackCounter++;
			}
			
			if (PointLocation[CurrentVertex] == COPLANAR_WITH_PLANE || 
                PointLocation[CurrentVertex] == PointLocation[i]) 
                continue;
			
			D3DXVECTOR3 IntersectPoint;
			float		percent;

			GetIntersect((D3DXVECTOR3*)&Poly->VertexList[i], 
                          (D3DXVECTOR3*)&Poly->VertexList[CurrentVertex], 
                          (D3DXVECTOR3*)&Plane->VertexList[0], 
                          &Plane->Normal, &IntersectPoint, &percent);

			VERTEX copy;
			float deltax = Poly->VertexList[CurrentVertex].tu - Poly->VertexList[i].tu;
			float deltay = Poly->VertexList[CurrentVertex].tv - Poly->VertexList[i].tv;
			float texx	 = Poly->VertexList[i].tu + (deltax * percent);
			float texy	 = Poly->VertexList[i].tv + (deltay * percent);
			copy.x		 = IntersectPoint.x;
            copy.y       = IntersectPoint.y;
            copy.z       = IntersectPoint.z;
			copy.rgba	 = Poly->VertexList[i].rgba;
			copy.tu		 = texx;
            copy.tv      = texy;

			BackList[BackCounter++]	  = copy;
			FrontList[FrontCounter++] = copy;
		}
	}

	

	FrontSplit->numVertices = 0;
	BackSplit->numVertices  = 0;

	
	FrontSplit->numVertices = FrontCounter;
	memcpy(FrontSplit->VertexList, FrontList, FrontCounter * sizeof(VERTEX));
	BackSplit->numVertices  = BackCounter;
	memcpy(BackSplit->VertexList, BackList, BackCounter * sizeof(VERTEX));

	BackSplit->numIndices	 = (BackSplit->numVertices  - 2) * 3;
	FrontSplit->numIndices = (FrontSplit->numVertices - 2) * 3;

	
	short IndxBase;
    
	short loop, v1, v2;
	for (loop = 0, v1 = 1, v2 = 2; 
	     loop < FrontSplit->numIndices/3; 
	     loop++, v1 = v2, v2++)
    {
		IndxBase = loop * 3;
		FrontSplit->Indices[ IndxBase    ] =  0;
		FrontSplit->Indices[ IndxBase + 1] = v1;
		FrontSplit->Indices[ IndxBase + 2] = v2;
	} 

	for (loop = 0, v1 = 1, v2 = 2; 
	     loop < BackSplit->numIndices/3; 
	     loop++, v1 = v2, v2++) 
    {
		IndxBase = loop * 3;
		BackSplit->Indices[ IndxBase    ] =  0;
		BackSplit->Indices[ IndxBase + 1] = v1;
		BackSplit->Indices[ IndxBase + 2] = v2;
	}

	
	FrontSplit->Normal = Poly->Normal;
	BackSplit->Normal  = Poly->Normal;
}

POLYGON *AddPolygon(POLYGON *Parent, VERTEX *Vertices, WORD numVerts)
{
	POLYGON *Child = new POLYGON;
    WORD v0;
    WORD v1;
    WORD v2;
    int	 i = 0;

	Child->numVertices = numVerts;
	Child->numIndices	 = (numVerts - 2) * 3;
	Child->Next				 = NULL;

	
	for (i = 0; i < numVerts; i++)
        Child->VertexList[i] = Vertices[i];

	for (i = 0; i < Child->numIndices / 3; i++)
    {
		if (i == 0)
        {
			v0 = 0;
            v1 = 1;
            v2 = 2;
        }
        else
        {
			v1 = v2;
            ++v2;
        }
	
		Child->Indices[ i * 3     ] = v0;
		Child->Indices[(i * 3) + 1] = v1;
		Child->Indices[(i * 3) + 2] = v2;
	} 

	
	D3DXVECTOR3 *vec0 = (D3DXVECTOR3*) &Child->VertexList[0];
	D3DXVECTOR3 *vec1 = (D3DXVECTOR3*) &Child->VertexList[1];
	D3DXVECTOR3 *vec2 = (D3DXVECTOR3*) &Child->VertexList[Child->numVertices-1];// the last vert

	D3DXVECTOR3 edge1 = (*vec1)-(*vec0);
	D3DXVECTOR3 edge2 = (*vec2)-(*vec0);
	D3DXVec3Cross(&Child->Normal,&edge1,&edge2);
	D3DXVec3Normalize(&Child->Normal,&Child->Normal);

	if (Parent != NULL)
        Parent->Next=Child;

	return Child;
}


POLYGON * SelectBestSplitter(POLYGON *PolyList)
{
	POLYGON *Splitter	  = PolyList;
	POLYGON *CurrentPoly  = NULL;
	POLYGON *SelectedPoly = NULL; 
	ULONG	 BestScore	  = 100000;

	while (Splitter != NULL) 
    {
		CurrentPoly = PolyList;
		ULONG score      = 0;
        ULONG splits     = 0;
        ULONG backfaces  = 0;
        ULONG frontfaces = 0;
		
		while (CurrentPoly != NULL) 
        {
			if (CurrentPoly != Splitter) 
            {
				int result = ClassifyPoly(Splitter, CurrentPoly);

				switch (result) 
                {
					case COPLANAR_WITH_PLANE:
						break;
					case IN_FRONT_OF_PLANE:
						frontfaces++;
						break;
					case BEHIND_PLANE:
						backfaces++;
						break;
					case STRADDLING_PLANE:
						splits++;
						break;
					default:
						break;
				}
			}
			CurrentPoly = CurrentPoly->Next;
		}
	 
		score = abs((int)(frontfaces-backfaces)) + (splits*4);

		if (score < BestScore) 
        {
			BestScore    = score;
			SelectedPoly = Splitter;
		}
  
		Splitter = Splitter->Next;
	}
	return SelectedPoly;
}
   
bool LineOfSight(D3DXVECTOR3 *Start, D3DXVECTOR3 *End, NODE *Node)
{
	D3DXVECTOR3 intersection;
    float		temp = 0.0f;

	if (Node->isLeaf == true)
        return !Node->isSolid;

	int PointA = ClassifyPoint(Start, Node->Splitter);
	int PointB = ClassifyPoint(End,   Node->Splitter);

	if (PointA == COPLANAR_WITH_PLANE && PointB == COPLANAR_WITH_PLANE) 
		return LineOfSight(Start,End,Node->Front);

	if (PointA == IN_FRONT_OF_PLANE && PointB == BEHIND_PLANE) 
    {
		GetIntersect(Start, End, (D3DXVECTOR3 *)&Node->Splitter->VertexList[0], &Node->Splitter->Normal, &intersection, &temp);
		return LineOfSight(Start, &intersection, Node->Front) && LineOfSight(End, &intersection, Node->Back); 
	}

	if (PointA == BEHIND_PLANE && PointB == IN_FRONT_OF_PLANE) 
    {
		GetIntersect(Start, End, (D3DXVECTOR3 *)&Node->Splitter->VertexList[0], &Node->Splitter->Normal, &intersection, &temp);
		return LineOfSight(End, &intersection, Node->Front) && LineOfSight(Start, &intersection, Node->Back);
	}

	if (PointA == IN_FRONT_OF_PLANE || PointB == IN_FRONT_OF_PLANE) 
		return LineOfSight(Start, End, Node->Front);
    else 
		return LineOfSight(Start, End, Node->Back);

	return true;
}

void DeleteBSPTree(NODE *Node)
{
	if (Node->Back) 
        DeleteBSPTree(Node->Back);

	if (Node->Front) 
        DeleteBSPTree(Node->Front);

	if (Node->Splitter) 
        delete Node->Splitter;

	delete Node;
}

VERTEX SetVertex(float x,  float y, float z,
				  float r,  float g, float b, float a,
				  float tu, float tv)
{
	VERTEX vertex;

	vertex.x	= x;
	vertex.y	= y;
	vertex.z	= z;
	vertex.rgba	= D3DCOLOR_COLORVALUE(r,g,b,a);
	vertex.tu	= tu;
	vertex.tv	= tv;
	
	return vertex;
}
