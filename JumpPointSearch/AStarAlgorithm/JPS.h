#pragma once

#include <list>
using namespace std;

#define TILE_WIDTH 15 // X��
#define TILE_HEIGHT 15 // Y��

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
	// Ÿ�� ��ġ
	int m_iXpos;
	int m_iYpos;

	st_Node *pParentNode;

	float m_F;
	float m_G;
	float m_H;

	// ī���� �߰��ؼ� �ִ� Ž������ ���ؾߵȴ�.
};

bool JumpPointSearch(int iStartX, int iStartY, int iDestX, int iDestY, BOOL bNewSearch);
BOOL CheckWalkable(int iX, int iY);
BOOL Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY);

BOOL CheckDirection(st_Node *pParent, int iX, int iY, int iDir);
void NeighborsNode(st_Node *pNode);
// OpenList�� CloseList�� ��ǥ�� �ִ��� Ȯ��
// �ִٸ� NextG�� ġȯ�����ִ� �Լ�
// Return: True ���� ������, False ���� ������������.
bool CheckAxis(int iX, int iY, float NextG, st_Node *pParent);




bool compNode(const st_Node* a, const st_Node *b);
void Clear();
void DrawMap();
void PrintPath();