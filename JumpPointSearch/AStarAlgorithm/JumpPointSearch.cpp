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
	// ����Ʈ�� �ִ� ��� ������ ����
	// ���� !! ���� OpenList���� st_Tile ��带 ����� ������
	
	// ���¸���Ʈ ����
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
	
	// ���� �ε����� ����.
	if (p_BlockMap == nullptr)
		return false;
	printf("==================================== JPS In Class ==================================== \n");
	memset(pArray, 0, sizeof(st_Point) * ArraySize);
	Clear();

	// ���۰� ���� ��带 �ʱ�ȭ�Ѵ�.
	InitNode(startX, startY, DestX, DestY);

	// OpenList�� �߰��Ѵ�.
	OpenList.insert(m_pStartPoint);
	m_iOpenCount++;

	while (1)
	{	
		if (OpenList.size() == 0)
			return false; // ���� ���

		if (m_iOpenCount >= m_iMaxOpen || m_iJumpCount >= m_iMaxJump)
			return false;

		st_Node *pNode = *OpenList.begin();
		OpenList.erase(OpenList.begin());
		CloseList.insert(pNode);
		printf("### OpenList Pop : Xpos : %d, Ypos : %d  F : %f \n", pNode->m_iXpos, pNode->m_iYpos, pNode->m_F);

		if (pNode->m_iXpos == m_pEndPoint->m_iXpos && pNode->m_iYpos == m_pEndPoint->m_iYpos)
		{
			m_pEndPoint->pParentNode = pNode->pParentNode;
			// �迭�� ���� ���� �ִ´�.
			// ���� ����
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
	// �θ� �������� �ʴ´ٸ� ���� Ž�������� ���� ����̴�. 
	// 8�������� �� ��Ʈ����.
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
		// ���� ���
		int parentX = pNode->pParentNode->m_iXpos;
		int parentY = pNode->pParentNode->m_iYpos;
		int Xpos = pNode->m_iXpos;
		int Ypos = pNode->m_iYpos;
		//------------------------------------------------------
		// Ž���� �Ǿ��� ����� �θ�� ���� � �������� �Դ����� Ȯ���Ͽ�
		// �Դ� ��δ� ���ư��� �ʴ´�.
		//------------------------------------------------------

		int DirX = parentX - Xpos;
		int DirY = parentY - Ypos;
		/*
		enDIR_RU �� ���  Xpos�� ����, Ypos�� ����
		enDIR_UU �� ���  Xpos�� 0, Ypos�� ����
		enDIR_LU �� ���  Xpos�� ��� Ypos�� ����

		enDIR_RR �� ��� Xpos�� ����  Ypos�� 0
		enDIR_LL �� ��� Xpos�� ���  Ypos �� 0

		enDIR_RD �� ��� Xpos�� ���� Ypos�� ���
		enDIR_DD �� ��� Xpos�� 0, Ypos�� ���
		enDIR_LD �� ��� Xpos�� ���, Ypos�� ���
		*/

		// enDIR_RR
		if (DirX < 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);
			// �ɼ�üũ RU, RD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		//enDIR_LL
		else if (DirX > 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			// �ɼ�üũ LU, LD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
		}
		// enDIR_UU
		else if (DirX == 0 && DirY > 0)
		{
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);
			// �ɼ�üũ LU, RU
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_DD
		else if (DirX == 0 && DirY < 0)
		{
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);
			// �ɼ�üũ LD, RD
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		//enDIR_RU
		else if (DirX < 0 && DirY > 0)
		{

			// �⺻���� UU, RU, RR
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);
			CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);

			// �ɼ�üũ LU, RD  
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}
		// enDIR_LU
		else if (DirX > 0 && DirY > 0)
		{

			// �⺻���� LL, LU, UU
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			CheckDirection(pNode, Xpos, Ypos - 1, enDIR_UU);

			// �ɼ�üũ LD, RU
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_RD
		else if (DirX < 0 && DirY < 0)
		{

			// �⺻���� RD, RR, DD
			CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
			CheckDirection(pNode, Xpos + 1, Ypos, enDIR_RR);
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);

			// �ɼ�üũ RU, LD 
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, enDIR_RU);
		}
		// enDIR_LD
		else if (DirX > 0 && DirY < 0)
		{
			// �⺻���� LD, LL, DD
			CheckDirection(pNode, Xpos - 1, Ypos + 1, enDIR_LD);
			CheckDirection(pNode, Xpos - 1, Ypos, enDIR_LL);
			CheckDirection(pNode, Xpos, Ypos + 1, enDIR_DD);

			// �ɼ�üũ LU, RD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, enDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, enDIR_RD);
		}

	}
}


// �� Ÿ�Ͽ� ���� �Ӽ�üũ��, Jump ����� True���, ������ -> OpenList�˻�, CloseList �˻� �ؼ� ������ ��¥ ����
BOOL CJumpPointSearch::CheckDirection(st_Node *pParent, int iX, int iY, int iDir)
{
	if (!CheckWalkable(iX, iY))
		return false;

	int JumpX = 0, JumpY = 0;  // ��� ���� X,Y ��ǥ

	if (Jump(iX, iY, iDir, &JumpX, &JumpY))
	{
		float NextG;
		// �밢���� ���
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

		// ���¸���Ʈ���� X,Y ��ǥ �˻��Ͽ� ��� �ִ��� Ȯ��, Ŭ�����Ʈ �˻�
		if (CheckAxis(JumpX, JumpY, NextG, pParent))
			return false; // �̹� �����ϴ� ���̱� ������ ���Ÿ� ��. 

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
		// Jump ����
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
		// Jump ����
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
		// Jump ����
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
		// Jump ����
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
		// Jump ����
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
		// Jump ����
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
	// ���۰� ���� ��带 �ʱ�ȭ�Ѵ�.
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


	// ���� pNext�� ���� �����Ѵ�.
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