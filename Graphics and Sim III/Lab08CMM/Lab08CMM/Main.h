// MAIN.H
// DESCRIPTION: IMPLEMENTS SOLID-LEAF BSP TREE

#ifndef MAIN_H
#define MAIN_H

#include "../Engine/Engine.h"

// C O N S T A N T S

enum
{
	IN_FRONT_OF_PLANE,
	BEHIND_PLANE,
	COPLANAR_WITH_PLANE,
	STRADDLING_PLANE
};

struct POLYGON 
{
	VERTEX	     VertexList[10];	
	D3DXVECTOR3  Normal;			
	int		     numVertices;	
	int		     numIndices;	
	WORD		 Indices[30];		
	POLYGON	    *Next;				
};

struct NODE	
{
	POLYGON	*Splitter; 
	NODE	*Front;    
	NODE	*Back;     
	bool	 isLeaf;  
	bool	 isSolid; 
};

// F U N C T I O N  P R O T O T Y P E S /////////////////////////////////////////////////////////

// Engine Control Functions -
void Init();               //
void Kill();               //
void OnLostDevice();       //
void OnResetDevice();      //
void Update(float dt);     //
void Draw();               //
// --------------------------

void SetD3DOptions();

VERTEX SetVertex(float x, float y, float z,float r, float g, float b, float a,
                 float tu, float tv);

// BSP FUNCTIONS
int ClassifyPoly(POLYGON *plane, POLYGON *Poly);
int ClassifyPoint(D3DXVECTOR3 *pos, POLYGON *Plane);
void InitLevelPolys();
void SplitPolygon(POLYGON *Poly,POLYGON *Plane, POLYGON *FrontSplit, POLYGON *BackSplit);
void BuildBspTree(NODE *CurrentNode, POLYGON *PolyList);
void WalkBspTree(NODE *Node, D3DXVECTOR3 *pos);
void DeleteBSPTree(NODE *Node);
bool LineOfSight(D3DXVECTOR3 *Start, D3DXVECTOR3 *End, NODE *Node);
bool GetIntersect(D3DXVECTOR3 *linestart, D3DXVECTOR3 *lineend, D3DXVECTOR3 *vertex,
				  D3DXVECTOR3 *normal, D3DXVECTOR3 *intersection, float *percentage);
POLYGON *AddPolygon(POLYGON *Parent, VERTEX *Vertices, WORD numVerts);
POLYGON *SelectBestSplitter(POLYGON *PolyList);

// BSP FUNCTIONS -----------------------ENDING





#endif

