// MAIN.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Main.h"

// G L O B A L S ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
Sound *g_music = 0;
Sound *g_sounds[10];

Terrain g_terrain[2];

int g_currShip = XWING;
Mesh g_shipMesh[NUM_SHIPS];
Mesh g_ringMesh;
Mesh g_arrowMesh;

int g_currTrack = 0;
Track g_track[NUM_TRACKS];

Font g_font;
Font g_smallFont;
char output[80];
float percentLoaded;

D3DXVECTOR3 g_cursorPosition;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	GUID guid =  
	{ 
		0x1b12317f, 
		0x39c9, 
		0x4920, 
		{ 0xa3, 0xa4, 0xf7, 0xa2, 0xc1, 0x60, 0x5a, 0x42 } 
	};

	EngineSetup setup;
	setup.instance = instance;
	setup.windowed = false;
	setup.name = "SPACE RACE 3000";
	setup.Init = Init;
	setup.Kill = Kill;
	setup.OnLostDevice = OnLostDevice;
	setup.OnResetDevice = OnResetDevice;
	setup.NetworkMessageHandler = NetworkMessageHandler;
	setup.guid = guid;
	
	new Engine(&setup);
	g_engine->Run();

	return(1);
}

void SetGlobalD3DOptions()
{
	// SET SAMPLING FILTER FOR TEXTURES TO LINEAR
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER,   D3DTEXF_ANISOTROPIC);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	g_engine->GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj, D3DX_PI/4, g_engine->GetWidth()/g_engine->GetHeight(), 1.0f, 100000.0f);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &proj);

	// SETUP A LIGHT SOURCE
	D3DLIGHT9 light;
	D3DXVECTOR3 dir(0.0f, -1.0f, 0.0);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	light = InitDirectionalLight(&dir, &color);
	g_engine->GetDevice()->SetLight(0, &light);
	g_engine->GetDevice()->LightEnable(0, true);
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);

}

void LoadingThread()
{
	Loading *loading = (Loading*)(g_engine->GetCurrentState());

	g_shipMesh[0].LoadXFile("Assets/Models/XWing.x");
	g_shipMesh[0].Scale(2.0f);

	//percentLoaded = 1.0f/10.0f;

	g_shipMesh[1].LoadXFile("Assets/Models/Sweepwing.x");
	g_shipMesh[1].Scale(2.0f);

	//percentLoaded = 2.0f/10.0f;

	g_shipMesh[2].LoadXFile("Assets/Models/Holewing.x");
	g_shipMesh[2].Scale(1.5f);

	//percentLoaded = 3.0f/10.0f;

	g_shipMesh[3].LoadXFile("Assets/Models/Roundwing.x");
	g_shipMesh[3].Scale(1.5f);

	//percentLoaded = 4.0f/10.0f;

	g_shipMesh[4].LoadXFile("Assets/Models/Splitwing.x");
	g_shipMesh[4].Scale(2.0f);

	//percentLoaded = 5.0f/10.0f;

	g_shipMesh[5].LoadXFile("Assets/Models/triwing.x");
	g_shipMesh[5].Scale(2.0f);

	//percentLoaded = 6.0f/10.0f;

	g_ringMesh.LoadXFile("Assets/Models/Ring.x");
	g_ringMesh.Scale(4.0f);

	//percentLoaded = 7.0f/10.0f;

	g_arrowMesh.LoadXFile("Assets/Models/Arrow.x");
	g_arrowMesh.Scale(0.5f);

	percentLoaded = 5.0f/10.0f;
	

	g_sounds[0] = new Sound("Assets/Sounds/ping.wav");
	g_sounds[1] = new Sound("Assets/Sounds/beep1.wav");
	g_sounds[2] = new Sound("Assets/Sounds/beep2.wav");
	g_sounds[3] = new Sound("Assets/Sounds/accessdenied.wav");
	g_sounds[4] = new Sound("Assets/Sounds/type.wav");
	g_sounds[5] = new Sound("Assets/Sounds/three.wav");
	g_sounds[6] = new Sound("Assets/Sounds/two.wav");
	g_sounds[7] = new Sound("Assets/Sounds/one.wav");
	g_sounds[8] = new Sound("Assets/Sounds/go.wav");
	g_sounds[9] = new Sound("Assets/Sounds/Captain Zerol.wav");

	g_terrain[0].Init(
		513, 513, 400.0f, 400.0f, 
		"Assets\\Heightmaps\\Heightmap2.raw", 
		"Assets\\Textures\\rock512.bmp", 
		"Assets\\Textures\\grass512.bmp", 
		"Assets\\Textures\\dirt512.bmp", 
		"Assets\\Textures\\sand512.bmp",
		D3DCOLOR_XRGB(0, 0, 255),
		150.0f);

	
	percentLoaded = 6.0f/10.0f;
	
	g_terrain[0].GenTexture(2048, 2048, "Assets\\Textures\\Map1Texture.bmp");

	percentLoaded = 7.0f/10.0f;

	g_terrain[1].Init(
		513, 513, 400.0f, 400.0f, 
		"Assets\\Heightmaps\\Heightmap1.raw", 
		"Assets\\Textures\\Snow2_Tile_512.bmp",
		"Assets\\Textures\\Ice1_TileImprove_512.bmp", 
		"Assets\\Textures\\Ice2_TileImprove_512.bmp", 
		"Assets\\Textures\\Ice4_TileImprove_512.bmp", 
		
		D3DCOLOR_XRGB(0, 0, 255),
		150.0f);

	percentLoaded = 8.0f/10.0f;

	g_terrain[1].GenTexture(2048, 2048, "Assets\\Textures\\Map2Texture.bmp");
	

	percentLoaded = 9.0f/10.0f;

	LoadTrack(&g_track[0], "Assets/Tracks/track1.txt");
	LoadTrack(&g_track[1], "Assets/Tracks/track2.txt");
	LoadTrack(&g_track[2], "Assets/Tracks/track3.txt");
	//ifstream inFile;
	//inFile.open("Assets/tracks/track1.txt");
	//int numRings;
	//inFile >> g_track[0].terrainId;
	////
	//inFile >> numRings;
	//g_track[0].rings.resize(numRings);
	//for (int i = 0; i < g_track[0].rings.size(); i++)
	//{
	//	D3DXVECTOR3 position;
	//	D3DXVECTOR3 right;
	//	D3DXVECTOR3 up;
	//	D3DXVECTOR3 look;
	//	
	//	inFile >> position.x >> position.y >> position.z;
	//	inFile >> right.x >> right.y >> right.z;
	//	inFile >> up.x >> up.y >> up.z;
	//	inFile >> look.x >> look.y >> look.z;

	//	g_track[0].rings[i].SetPosition(position);
	//	g_track[0].rings[i].SetRight(right);
	//	g_track[0].rings[i].SetUp(up);
	//	g_track[0].rings[i].SetLook(look);
	//}
	//inFile.close();

	percentLoaded = 1.0f;
		//percentLoaded = ((float)z*16.0f+x+1)/256.0f;
			
	
	loading->SetReady(true);
}

void Init()
{
	g_font.Init("Assets/Textures/font.dds", 32);
	g_smallFont.Init("Assets/Textures/smallfont.dds", 16);

	g_engine->AddState(new Loading);
	g_engine->AddState(new MainMenu, false);
	g_engine->AddState(new TimeTrial, false);
	g_engine->AddState(new Multiplayer, false);
	g_engine->AddState(new EditTrack, false);
	/*
	g_engine->AddState(new MultiPlayerGame, false);
	g_engine->AddState(new EditMap, false);*/

	/*g_music = new Sound("assets\\corneria.wav");
	g_music->Play(true);*/

	SetGlobalD3DOptions();
	
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LoadingThread, 0, 0, 0);
}

void Kill()
{
	for (int i = 0; i < 10; i++)
		SAFE_DELETE(g_sounds[i]);
	//g_music->GetSegment()->Unload(0);
	//SAFE_DELETE(g_music);
}

void OnLostDevice()
{
	
}

void OnResetDevice()
{
	SetGlobalD3DOptions();
}


void NetworkMessageHandler(ReceivedMessage *msg)
{
	if (g_engine->GetCurrentState() != NULL)
	{
		if (g_engine->GetCurrentState()->GetID() != MULTIPLAYER_STATE)
			return;
	}
	else return;

	((Multiplayer*)(g_engine->GetCurrentState()))->NetworkMessageHandler(msg);
}

void LoadTrack(Track *track, char *filename)
{
	ifstream inFile;
	inFile.open(filename);
	int numRings;
	inFile >> track->color;
	inFile >> track->terrainId;
	inFile >> numRings;

	track->rings.resize(numRings);
	for (int i = 0; i < track->rings.size(); i++)
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 right;
		D3DXVECTOR3 up;
		D3DXVECTOR3 look;
		
		inFile >> position.x >> position.y >> position.z;
		inFile >> right.x >> right.y >> right.z;
		inFile >> up.x >> up.y >> up.z;
		inFile >> look.x >> look.y >> look.z;

		track->rings[i].SetPosition(position);
		track->rings[i].SetRight(right);
		track->rings[i].SetUp(up);
		track->rings[i].SetLook(look);
	}
	inFile.close();
}

void SaveTrack(Track track, char *filename)
{
	ofstream outFile;
	outFile.open(filename, ios_base::out | ios_base::trunc);
	outFile << track.color << endl;
	outFile << track.terrainId << endl;
	outFile << track.rings.size() << endl;
	for (int i = 0; i < track.rings.size(); i++)
	{
		D3DXVECTOR3 position = track.rings[i].GetPosition();
		D3DXVECTOR3 right = track.rings[i].GetRight();
		D3DXVECTOR3 up = track.rings[i].GetUp();
		D3DXVECTOR3 look = track.rings[i].GetLook();

		outFile << position.x << " " << position.y << " " << position.z << endl;
		outFile << right.x << " " << look.y << " " << right.z << endl;
		outFile << up.x << " " << up.y << " " << up.z << endl;
		outFile << look.x << " " << look.y << " " << look.z << endl;
	}
	outFile.close();

}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////