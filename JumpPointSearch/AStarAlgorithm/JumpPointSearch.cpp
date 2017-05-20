#include "stdafx.h"
#include "JumpPointSearch.h"
#include <iostream>
using namespace std;


CJumpPointSearch::CJumpPointSearch(int Width, int Height, int MaxOpen, int MaxJump)
	:m_iWidth(Width), m_iHeight(Height), m_iMaxOpen(MaxOpen), m_iMaxJump(MaxJump)
{
	m_iOpenCount = 0;
	m_iJumpCount = 0;

}


CJumpPointSearch::~CJumpPointSearch()
{
	// 리스트에 있는 모든 데이터 삭제
	// 주의 !! 먼저 OpenList안의 st_Tile 노드를 지우고 지워라
	
	// 오픈리스트 제거
	Clear();
}

bool CJumpPointSearch::MapOpen(st_Tile (*pMap))
{
	if (pMap == nullptr)
		return false;

	
	p_BlockMap = pMap;
	return true;
}
bool CJumpPointSearch::PathFind(int startX, int startY, int DestX, int DestY, st_Point *pArray, int ArraySize)
{
	
	// 맵을 로드하지 않음.
	if (p_BlockMap == nullptr)
		return false;
	printf("==================================== JPS In Class ==================================== \n");
	memset(pArray, 0, sizeof(st_Point) * ArraySize);
	Clear();

	// 시작과 끝의 노드를 초기화한다.
	InitNode(startX, startY, DestX, DestY);

	// OpenList에 추가한다.
	OpenList.insert(m_pStartPoint);
	m_iOpenCount++;

	while (1)
	{	
		if (OpenList.size() == 0)
			return false; // 종료 대상

		if (m_iOpenCount >= m_iMaxOpen || m_iJumpCount >= m_iMaxJump)
			return false;

		st_Node *pNode = *OpenList.begin();
		OpenList.erase(OpenList.begin());
		CloseList.insert(pNode);
		printf("### OpenList Pop : Xpos : %d, Ypos : %d  F : %f \n", pNode->m_iXpos, pNode->m_iYpos, pNode->m_F);

		if (pNode->m_iXpos == m_pEndPoint->m_iXpos && pNode->m_iYpos == m_pEndPoint->m_iYpos)
		{
			m_pEndPoint->pParentNode = pNode->pParentNode;
			// 배열에 값을 집어 넣는다.
			// 종료 조건
			InputArray(pArray, ArraySize);
			return true;
		}
		NeighborsNode(pNode);

	}
	return false;
}

void CJumpPointSearch::NeighborsNode(st_Node *pNode)
{
	//------------------------------------------------------
	// 부모가 존재하지 않는다면 아직 탐색된적이 없는 노드이다. 
	// 8방향으로 쫙 퍼트린다.
	//------------------------------------------------------
	if (pNode->pParentNode == nullptr)
	{
		int Xpos = pNode->m_iXpos;
		int Ypos = pNode->m_iYpos;

		CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
		CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);
		CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);

		CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
		CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);

		CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
		CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);
		CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
	}
	else
	{
		// 방향 계산
		int parentX = pNode->pParentNode->m_iXpos;
		int parentY = pNode->pParentNode->m_iYpos;
		int Xpos = pNode->m_iXpos;
		int Ypos = pNode->m_iYpos;
		//------------------------------------------------------
		// 탐색이 되었던 노드라면 부모로 부터 어떤 방향으로 왔는지를 확인하여
		// 왔던 길로는 돌아가지 않는다.
		//------------------------------------------------------

		int DirX = parentX - Xpos;
		int DirY = parentY - Ypos;
		/*
		enDIR_RU 인 경우  Xpos도 음수, Ypos도 음수
		enDIR_UU 인 경우  Xpos는 0, Ypos는 음수
		enDIR_LU 인 경우  Xpos는 양수 Ypos는 음수

		enDIR_RR 인 경우 Xpos는 음수  Ypos는 0
		enDIR_LL 인 경우 Xpos는 양수  Ypos 는 0

		enDIR_RD 인 경우 Xpos는 음수 Ypos는 양수
		enDIR_DD 인 경우 Xpos는 0, Ypos는 양수
		enDIR_LD 인 경우 Xpos는 양수, Ypos는 양수
		*/

		// enDIR_RR
		if (DirX < 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);
			// 옵션체크 RU, RD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		//enDIR_LL
		else if (DirX > 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			// 옵션체크 LU, LD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
		}
		// enDIR_UU
		else if (DirX == 0 && DirY > 0)
		{
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);
			// 옵션체크 LU, RU
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_DD
		else if (DirX == 0 && DirY < 0)
		{
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);
			// 옵션체크 LD, RD
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		//enDIR_RU
		else if (DirX < 0 && DirY > 0)
		{

			// 기본방향 UU, RU, RR
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);
			CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);

			// 옵션체크 LU, RD  
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		// enDIR_LU
		else if (DirX > 0 && DirY > 0)
		{

			// 기본방향 LL, LU, UU
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);

			// 옵션체크 LD, RU
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_RD
		else if (DirX < 0 && DirY < 0)
		{

			// 기본방향 RD, RR, DD
			CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);

			// 옵션체크 RU, LD 
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_LD
		else if (DirX > 0 && DirY < 0)
		{
			// 기본방향 LD, LL, DD
			CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);

			// 옵션체크 LU, RD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}

	}
}


// 현 타일에 대한 속성체크후, Jump 결과가 True라면, 노드생성 -> OpenList검사, CloseList 검사 해서 없으면 진짜 생성
BOOL CJumpPointSearch::CheckDirection(st_Node *pParent, int iX, int iY, int iDir)
{
	if (!CheckWalkable(iX, iY))
		return false;

	int JumpX = 0, JumpY = 0;  // 노드 생성 X,Y 좌표

	if (Jump(iX, iY, iDir, &JumpX, &JumpY))
	{
		float NextG;
		// 대각선인 경우
		switch (iDir)
		{
		case enDIR_RU:
		case enDIR_LU:
		case enDIR_RD:
		case enDIR_LD:
			NextG = pParent->m_G + 1.15 * (abs(pParent->m_iXpos - iX) + abs(pParent->m_iYpos - iY));
			break;
		default:
			NextG = pParent->m_G + (abs(pParent->m_iXpos - iX) + abs(pParent->m_iYpos - iY));
		}

		// 오픈리스트에서 X,Y 좌표 검색하여 노드 있는지 확인, 클로즈리스트 검사
		if (CheckAxis(JumpX, JumpY, NextG, pParent))
			return false; // 이미 존재하는 값이기 때문에 갱신만 함. 

		st_Node *newNode = new st_Node;
		newNode->m_iXpos = JumpX;
		newNode->m_iYpos = JumpY;
		newNode->pParentNode = pParent;
		newNode->m_G = NextG;
		newNode->m_H = abs(m_pEndPoint->m_iXpos - JumpX) + abs(m_pEndPoint->m_iYpos - JumpY);
		newNode->m_F = newNode->m_G + newNode->m_H;

		OpenList.insert(newNode);
		m_iOpenCount++;

		return true;
	}
	return false;

}


BOOL CJumpPointSearch::Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY)
{
	switch (iDir)
	{
	case enDIR_DD:
		return Jump_DD(iX, iY, pJumpX, pJumpY);
	case enDIR_RR:
		return Jump_RR(iX, iY, pJumpX, pJumpY);
	case enDIR_UU:
		return Jump_UU(iX, iY, pJumpX, pJumpY);
	case enDIR_LL:
		return Jump_LL(iX, iY, pJumpX, pJumpY);
	case enDIR_LU:
		return Jump_LU(iX, iY, pJumpX, pJumpY);
	case enDIR_RU:
		return Jump_RU(iX, iY, pJumpX, pJumpY);
	case enDIR_RD:
		return Jump_RD(iX, iY, pJumpX, pJumpY);
	case enDIR_LD:
		return Jump_LD(iX, iY, pJumpX, pJumpY);
	}

}


bool CJumpPointSearch::Jump_DD(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX - 1, iY) && CheckWalkable(iX - 1, iY + 1)) ||
			(!CheckWalkable(iX + 1, iY) && CheckWalkable(iX + 1, iY + 1)) || 
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iY++;
	}
	return false;
}


bool CJumpPointSearch::Jump_RR(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY - 1) && CheckWalkable(iX + 1, iY - 1)) ||
			(!CheckWalkable(iX, iY + 1) && CheckWalkable(iX + 1, iY + 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iX++;
	}
	return false;
}

bool CJumpPointSearch::Jump_UU(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX - 1, iY) && CheckWalkable(iX - 1, iY - 1)) ||
			(!CheckWalkable(iX + 1, iY) && CheckWalkable(iX + 1, iY - 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iY--;
	}
	return false;
}

bool CJumpPointSearch::Jump_LL(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY - 1) && CheckWalkable(iX - 1, iY - 1)) ||
			(!CheckWalkable(iX, iY + 1) && CheckWalkable(iX - 1, iY + 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iX--;
	}

	return false;
}

bool CJumpPointSearch::Jump_LU(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY + 1) && CheckWalkable(iX - 1, iY + 1)) ||
			(!CheckWalkable(iX + 1, iY) && CheckWalkable(iX + 1, iY - 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		if (Jump_LL(iX - 1, iY, pJumpX, pJumpY) || Jump_UU(iX, iY - 1, pJumpX, pJumpY))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iX--;
		iY--;
	}

	return false;
}

bool CJumpPointSearch::Jump_RU(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY + 1) && CheckWalkable(iX + 1, iY + 1)) ||
			(!CheckWalkable(iX - 1, iY) && CheckWalkable(iX - 1, iY - 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		if (Jump_RR(iX + 1, iY, pJumpX, pJumpY) || Jump_UU(iX, iY - 1, pJumpX, pJumpY))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}
		// Jump 증가
		m_iJumpCount++;
		iX++;
		iY--;
	}

	return false;
}

bool CJumpPointSearch::Jump_RD(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY - 1) && CheckWalkable(iX + 1, iY - 1)) ||
			(!CheckWalkable(iX - 1, iY) && CheckWalkable(iX - 1, iY + 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		if (Jump_RR(iX + 1, iY, pJumpX, pJumpY) || Jump_DD(iX, iY + 1, pJumpX, pJumpY))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		m_iJumpCount++;
		iX++;
		iY++;
	}

	return false;
}

bool CJumpPointSearch::Jump_LD(int iX, int iY, int *pJumpX, int *pJumpY)
{
	while (CheckWalkable(iX, iY))
	{
		if ((!CheckWalkable(iX, iY - 1) && CheckWalkable(iX - 1, iY - 1)) ||
			(!CheckWalkable(iX + 1, iY) && CheckWalkable(iX + 1, iY + 1)) ||
			(iX == m_pEndPoint->m_iXpos && iY == m_pEndPoint->m_iYpos))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		if (Jump_LL(iX - 1, iY, pJumpX, pJumpY) || Jump_DD(iX, iY + 1, pJumpX, pJumpY))
		{
			*pJumpX = iX;
			*pJumpY = iY;
			return true;
		}

		m_iJumpCount++;
		iX--;
		iY++;
	}

	return false;
}

void CJumpPointSearch::InitNode(int iStartX, int iStartY, int iEndX, int iEndY)
{
	// 시작과 끝의 노드를 초기화한다.
	m_pStartPoint = new st_Node;
	m_pEndPoint = new st_Node;

	m_pStartPoint->m_iXpos = iStartX;
	m_pStartPoint->m_iYpos = iStartY;

	m_pStartPoint->pNext = nullptr;
	m_pStartPoint->pParentNode = nullptr;

	m_pStartPoint->m_G = 0;
	m_pStartPoint->m_H = abs(iStartX - iEndX) + abs(iStartY - iEndY);
	m_pStartPoint->m_F = m_pStartPoint->m_G + m_pStartPoint->m_H;

	m_pEndPoint->m_iXpos = iEndX;
	m_pEndPoint->m_iYpos = iEndY;
	m_pEndPoint->pParentNode = nullptr;
	m_pEndPoint->pNext = nullptr;
}

bool CJumpPointSearch::CheckAxis(int iX, int iY, float NextG, st_Node *pParent)
{	
	NODE_FINDER Find(iX, iY);
	multiset<st_Node *>::iterator iter;

	iter = find_if(OpenList.begin(), OpenList.end(), Find);

	if (iter != OpenList.end())
	{
		st_Node *pNode = (*iter);
		if (pNode->m_G > NextG)
		{
			pNode->pParentNode = pParent;
			pNode->m_G = NextG;
			pNode->m_F = pNode->m_G + pNode->m_H;
		}
		return true;
	}


	iter = find_if(CloseList.begin(), CloseList.end(), Find);

	if (iter != CloseList.end())
	{
		st_Node *pNode = (*iter);
		if (pNode->m_G > NextG)
		{
			pNode->pParentNode = pParent;
			pNode->m_G = NextG;
			pNode->m_F = pNode->m_G + pNode->m_H;

			CloseList.erase(iter);
			OpenList.insert(pNode);
		}
		return true;
	}

	return false;

}


void CJumpPointSearch::InputArray(st_Point *pArray, int ArraySize)
{
	st_Node *pNode = m_pEndPoint;


	// 먼저 pNext를 전부 연결한다.
	while (pNode != m_pStartPoint)
	{
		pNode->pParentNode->pNext = pNode;
		pNode = pNode->pParentNode;
	}

	int i = 0;
	pNode = m_pStartPoint;
		for (; i < ArraySize; i++)
		{

			if (pNode == nullptr)
				break;
			
			pArray[i].X = pNode->m_iXpos;
			pArray[i].Y = pNode->m_iYpos;

			pNode = pNode->pNext;
		}


		printf("### Operation Result ### \n");
		printf("### OpenCount = %d, JumpCount = %d \n", m_iOpenCount, m_iJumpCount);
}


void CJumpPointSearch::Clear()
{
	delete m_pStartPoint;
	delete m_pEndPoint;

	multiset<st_Node *>::iterator iter = OpenList.begin();

	for (; iter != OpenList.end();)
	{
	//	delete (*iter);
		iter = OpenList.erase(iter);
	}

	for (iter = CloseList.begin(); iter != CloseList.end();)
	{
	//	delete (*iter);
		iter = CloseList.erase(iter);
	}
}