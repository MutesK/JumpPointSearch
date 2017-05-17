#pragma once
/*
#define TILE_WIDTH 15 // X축
#define TILE_HEIGHT 15 // Y축


#define MAP_WIDTH 45
#define MAP_HEIGHT 80

#define dfEMPTY 0
#define dfOBSTACLE 1


struct st_Tile
{
	int m_Xpos;
	int m_Ypos;

	int Type;
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
};
// 각 주소는 실주소가 아닌 타일주소를 넘긴다.
// Return : True 끝 False 미완성
bool PathFind(int startX, int startY, int endX, int endY);
void AxisEightAddNode(st_Node *pParent);

// 실제로 만들수있는가의 판단여부이다. 이미 만들어져 있든 상관없다.
// 단, 시작점은 여기서 스킵하도록 하자.
bool TileCheck(int Xpos, int Ypos);
void MakeNode(st_Node *pParent, int X, int Y);
void DrawMap();

bool compNode(const st_Node* a, const st_Node *b);

void Clear();
void PrintPath();
*/