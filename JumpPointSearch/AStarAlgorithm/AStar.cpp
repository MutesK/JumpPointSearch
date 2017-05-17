/*
#include "stdafx.h"
#include "AStar.h"
#include <list>
#include <queue>
using namespace std;
extern HDC g_hDC;
extern RECT rect;
extern BYTE BlockMap[][MAP_HEIGHT];
extern st_Node* startPoint;
extern st_Node* endPoint;

bool isFirst = true;
bool isSuccess = false;
list<st_Node *> OpenList;
list<st_Node *> CloseList;

bool PathFind(int startX, int startY, int endX, int endY)
{
	if (isSuccess)
	{
		isSuccess = false;
		isFirst = true;
	}

	if (isFirst)
	{
		// 시작노드 OpenList에 삽입
		startPoint->m_iXpos = startX;
		startPoint->m_iYpos = startY;

		startPoint->m_G = 0;
		startPoint->m_H = abs(startX - endX) + abs(startY - endY);
		startPoint->m_F = startPoint->m_G + startPoint->m_H;
		OpenList.push_back(startPoint);
		isFirst = false;
		return false;
	}
	else
	{
		// 오픈리스트에서 F가 가장 작은 노드를 뽑느다.
		OpenList.sort(compNode);
		st_Node *pNode = (*OpenList.begin());
		if (OpenList.size() == 0)
			return true;

		OpenList.erase(OpenList.begin());
		CloseList.push_back(pNode);
		//	이 노드가 목적지라면?
		if (pNode->m_iXpos == endPoint->m_iXpos && pNode->m_iYpos == endPoint->m_iYpos)
		{
			endPoint->pParentNode = pNode->pParentNode;
			isSuccess = true;
			return true;
		}
		AxisEightAddNode(pNode);
	}
	return false;
}

void AxisEightAddNode(st_Node *pParent)
{
	int Xpos = pParent->m_iXpos;
	int Ypos = pParent->m_iYpos;

	
	if (TileCheck(Xpos - 1, Ypos - 1))
		MakeNode(pParent, Xpos - 1, Ypos - 1);
	if (TileCheck(Xpos, Ypos - 1))
		MakeNode(pParent, Xpos, Ypos - 1);
	if (TileCheck(Xpos + 1, Ypos - 1))
		MakeNode(pParent, Xpos + 1, Ypos - 1);

	if (TileCheck(Xpos - 1, Ypos))
		MakeNode(pParent, Xpos - 1, Ypos);
	if (TileCheck(Xpos + 1, Ypos))
		MakeNode(pParent, Xpos + 1, Ypos);

	if (TileCheck(Xpos - 1, Ypos + 1))
		MakeNode(pParent, Xpos - 1, Ypos + 1);
	if (TileCheck(Xpos, Ypos + 1))
		MakeNode(pParent, Xpos, Ypos + 1);
	if (TileCheck(Xpos + 1, Ypos + 1))
		MakeNode(pParent, Xpos + 1, Ypos + 1);
}

bool TileCheck(int Xpos, int Ypos)
{
	if (Xpos < 0 || Xpos >=  MAP_HEIGHT)
		return false;

	if (Ypos < 0 || Ypos >=  MAP_WIDTH)
		return false;


	// 장애물이 있는지 확인한다.
	if (BlockMap[Ypos][Xpos] == dfOBSTACLE)
		return false;

	return true;
}

void MakeNode(st_Node *pParent, int X, int Y)
{
	int ParentX = pParent->m_iXpos;
	int ParentY = pParent->m_iYpos;

	float NextG = pParent->m_G;
	// 대각선인 경우
	if (ParentX - 1 == X && ParentY - 1 == Y)
		NextG += 1.15;
	else if(ParentX + 1 == X && ParentY - 1 == Y)
		NextG += 1.15;
	else if(ParentX - 1 == X && ParentY + 1 == Y)
		NextG += 1.15;
	else if (ParentX + 1 == X && ParentY + 1 == Y)
		NextG += 1.15;
	else 
		NextG += 1;

	// 오픈리스트에서 X,Y 좌표 검색하여 노드 있는지 확인
	list<st_Node *>::iterator iter;
	for (iter = OpenList.begin(); iter != OpenList.end(); ++iter)
	{
		if ((*iter)->m_iXpos  > X || (*iter)->m_iXpos  < X)
			continue;

		if ((*iter)->m_iYpos  > Y || (*iter)->m_iYpos < Y)
			continue;

		if (X == (*iter)->m_iXpos && Y == (*iter)->m_iYpos)
		{
			st_Node *pNode = (*iter);
			if (pNode->m_G > NextG)
			{
				pNode->pParentNode = pParent;
				pNode->m_G = NextG;
				pNode->m_F = pNode->m_G + pNode->m_H;
			}
			return;
		}
	}

	// 클로즈리스트에서 X,Y 좌표 검색하여 노드 있는지 확인
	for (iter = CloseList.begin(); iter != CloseList.end(); ++iter)
	{
		if ((*iter)->m_iXpos  > X || (*iter)->m_iXpos  < X)
			continue;

		if ((*iter)->m_iYpos > Y || (*iter)->m_iYpos  < Y)
			continue;

		if (X == (*iter)->m_iXpos && Y == (*iter)->m_iYpos)
		{
			st_Node *pNode = (*iter);
			if (pNode->m_G > NextG)
			{
				pNode->pParentNode = pParent;
				pNode->m_G = NextG;
				pNode->m_F = pNode->m_G + pNode->m_H;
			}
			// 클로즈리스트에서 제거한다.
			CloseList.erase(iter);
			OpenList.push_back(pNode);
			return;
		}
	}

	st_Node* newNode = new st_Node;
	newNode->pParentNode = pParent;
	newNode->m_iXpos = X;
	newNode->m_iYpos = Y;
	newNode->m_G = NextG;
	newNode->m_H = float(abs(X - endPoint->m_iXpos) + abs(Y - endPoint->m_iYpos));
	newNode->m_F = newNode->m_G + newNode->m_H;
	OpenList.push_back(newNode);
}

void DrawMap()
{
	PatBlt(g_hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
	int Xpos = 0;
	int Ypos = 0;

	HPEN hPen, oldPen;
	hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	oldPen = (HPEN)SelectObject(g_hDC, hPen);

	// 세로 선을 그린다.
	for (int y = 0; y <= MAP_HEIGHT; y++)
	{
		MoveToEx(g_hDC, Xpos, Ypos, NULL);
		LineTo(g_hDC, Xpos, TILE_HEIGHT * MAP_WIDTH);
		Xpos += TILE_WIDTH;
	}

	// 가로 선을 그린다.
	Xpos = 0;
	for (int x = 0; x <= MAP_WIDTH; x++)
	{
		MoveToEx(g_hDC, Xpos, Ypos, NULL);
		LineTo(g_hDC, TILE_WIDTH * MAP_HEIGHT, Ypos);
		Ypos += TILE_HEIGHT;
	}


	///////////////////////////////////////////////////////////////////////////
	HBRUSH hRedBrush;
	HBRUSH hGreenBrush;
	HBRUSH hGrayBrush;
	HBRUSH hYelloBrush; // CloseList
	HBRUSH hBlueBrush; // OpenList
	HBRUSH hOldBrush;

	hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	hGrayBrush = CreateSolidBrush(RGB(105, 105, 105));
	hYelloBrush = CreateSolidBrush(RGB(255, 255, 0));
	hBlueBrush = CreateSolidBrush(RGB(0, 191,  255));

	std::list<st_Node *>::iterator iter;

	// OpenList 출력
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hBlueBrush);
	for (iter = OpenList.begin(); iter != OpenList.end(); iter++)
	{
		Xpos = (*iter)->m_iXpos * TILE_WIDTH;
		Ypos = (*iter)->m_iYpos * TILE_HEIGHT;
		Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	}
	DeleteObject(hBlueBrush);

	// CloseList 출력
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hYelloBrush);
	for (iter = CloseList.begin(); iter != CloseList.end(); iter++)
	{
		Xpos = (*iter)->m_iXpos * TILE_WIDTH;
		Ypos = (*iter)->m_iYpos * TILE_HEIGHT;
		Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	}
	DeleteObject(hYelloBrush);

	// 출발
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hGreenBrush);
	Xpos = startPoint->m_iXpos * TILE_WIDTH;
	Ypos = startPoint->m_iYpos * TILE_HEIGHT;
	Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	DeleteObject(hGreenBrush);

	// 목적지
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hRedBrush);
	Xpos = endPoint->m_iXpos * TILE_WIDTH;
	Ypos = endPoint->m_iYpos * TILE_HEIGHT;
	Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	DeleteObject(hRedBrush);

	// 방해물
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hGrayBrush);
	int X = 0, Y = 0;
	for (Y = 0; Y < MAP_WIDTH; Y++)
	{
		for (X = 0; X < MAP_HEIGHT; X++)
		{
			if (BlockMap[Y][X] == dfOBSTACLE)
			{
				Xpos = X * TILE_WIDTH;
				Ypos = Y * TILE_HEIGHT;
				Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
			}
		}
	}
	DeleteObject(hGrayBrush);

	DeleteObject(hPen);
	SelectObject(g_hDC, hOldBrush);
	SelectObject(g_hDC, oldPen);

	PrintPath();
}

bool compNode(const st_Node* a, const st_Node *b)
{
	return a->m_F < b->m_F;
}

void Clear()
{
	OpenList.clear();
	CloseList.clear();
	isFirst = true;
}

void PrintPath()
{
	if (isSuccess)
	{
		st_Node *pNode = endPoint;
		HPEN hPen, oldPen;
		hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		oldPen = (HPEN)SelectObject(g_hDC, hPen);

		while (pNode != startPoint)
		{
			// 사각형 가운데에서 다음 사각형 가운데로 선을 긋는다.
			int StartXpos = pNode->m_iXpos * TILE_WIDTH + TILE_WIDTH / 2;
			int StartYpos = pNode->m_iYpos * TILE_HEIGHT + TILE_HEIGHT / 2;
			int EndXpos = pNode->pParentNode->m_iXpos * TILE_WIDTH + TILE_WIDTH / 2;
			int EndYpos = pNode->pParentNode->m_iYpos * TILE_HEIGHT + TILE_HEIGHT / 2;

			MoveToEx(g_hDC, StartXpos, StartYpos, NULL);
			LineTo(g_hDC, EndXpos, EndYpos);

			pNode = pNode->pParentNode;
		}

		DeleteObject(hPen);
		SelectObject(g_hDC, oldPen);
	}
}*/