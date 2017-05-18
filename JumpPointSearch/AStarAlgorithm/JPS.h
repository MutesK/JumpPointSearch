#pragma once

#include <list>
using namespace std;

#define TILE_WIDTH 15 // X축
#define TILE_HEIGHT 15 // Y축

#define MAP_WIDTH 45
#define MAP_HEIGHT 80

#define dfEMPTY 0
#define dfOBSTACLE 1


#define dfDIR_RU 10
#define dfDIR_UU 11
#define dfDIR_LU 12
#define dfDIR_RR 13
#define dfDIR_LL 14
#define dfDIR_RD 15
#define dfDIR_DD 16
#define dfDIR_LD 17


struct st_Tile
{
	int Type;
	BYTE RColor;
	BYTE GColor;
	BYTE BColor;
};

struct st_Jump
{
	int iX;
	int iY;
	int iDir;
	int JumpX;
	int JumpY;
};

struct st_Node
{
	// 타일 위치
	int m_iXpos;
	int m_iYpos;

	st_Node *pParentNode;

	float m_F;
	float m_G;
	float m_H;

	// 카운터 추가해서 최대 탐색점을 구해야된다.
};

bool JumpPointSearch(int iStartX, int iStartY, int iDestX, int iDestY, BOOL bNewSearch);
BOOL CheckWalkable(int iX, int iY);
BOOL Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY);

BOOL CheckDirection(st_Node *pParent, int iX, int iY, int iDir);
void NeighborsNode(st_Node *pNode);
// OpenList와 CloseList에 좌표가 있는지 확인
// 있다면 NextG로 치환시켜주는 함수
// Return: True 값이 존재함, False 값이 존재하지않음.
bool CheckAxis(int iX, int iY, float NextG, st_Node *pParent);




bool compNode(const st_Node* a, const st_Node *b);
void Clear();
void DrawMap();
void PrintPath();