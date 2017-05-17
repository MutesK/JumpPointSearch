#include "stdafx.h"
#include "JPS.h"
#include <time.h> 
#include <stack>
extern HDC g_hDC;
extern RECT rect;
extern st_Tile BlockMap[][MAP_HEIGHT];
extern st_Node* startPoint;
extern st_Node* endPoint;

bool isFirst = true;
bool isSuccess = false;
list<st_Node *> OpenList;
list<st_Node *> CloseList;
BYTE lR_byCheckColor = 0;
BYTE lG_byCheckColor = 0;
BYTE lB_byCheckColor = 0;


stack<st_Jump> JumpStack;
//------------------------------------------------------
// ��ã�� ���� �Լ�.
//
// API �� ã�ư��� ������ �׷��ִ� ���� �����̹Ƿ�, 
// �� �Լ��� �������� ������ ã�� �� ���� ������ ���� �ʴ´�.
//
// Open ������ ��� �ϳ��� ���� 1ȸ ��ã�� �۾� �� ���ϵ�.
// �� �� �ݺ������� �� �Լ��� ȣ���Ͽ� 1�ܰ辿 ��ã�� �۾��� ����.
//
// bNewSearch �� TRUE ��� ó������ �˻��� �����ϸ�,
// bNewSearch �� FALSE ��� ������ ��ã�� �����͸� �̾ ����.
//
// ��ã�Ⱑ ����Ǹ� TRUE �� �����ϸ�,  ��� �����ؾ� �ϴ� ���� FALSE �� ����.
//------------------------------------------------------
bool JumpPointSearch(int iStartX, int iStartY, int iDestX, int iDestY, BOOL bNewSearch)
{
	/*
		A* �� �Ȱ��� ������� Open ��� �̾Ƽ�
		NeighborsNode �Լ��� �ֺ� ��� ����!
	*/
	if (isSuccess)
	{
		isSuccess = false;
		isFirst = true;
	}

	if (isFirst)
	{
		startPoint->m_iXpos = iStartX;
		startPoint->m_iYpos = iStartY;

		startPoint->pParentNode = nullptr;
		startPoint->m_G = 0;
		startPoint->m_H = abs(iStartX - iDestX) + abs(iStartY - iDestY);
		startPoint->m_F = startPoint->m_G + startPoint->m_H;
		OpenList.push_back(startPoint);
		isFirst = false;

		endPoint->m_iXpos = iDestX;
		endPoint->m_iYpos = iDestY;
		return false;
	}
	else
	{
		OpenList.sort(compNode);
		st_Node *pNode = (*OpenList.begin());
		if (OpenList.size() == 0)
			return true;

		OpenList.erase(OpenList.begin());
		CloseList.push_back(pNode);
		if (pNode->m_iXpos == endPoint->m_iXpos && pNode->m_iYpos == endPoint->m_iYpos)
		{
			endPoint->pParentNode = pNode->pParentNode;
			isSuccess = true;
			return true;
		}
		srand(time(NULL));
		lR_byCheckColor += max(rand() % 50 % 256, 20);
		lG_byCheckColor += max(rand() % 50 % 256, 20);
		lB_byCheckColor += max(rand() % 50 % 256, 20);
		NeighborsNode(pNode);
	}

	return false;
}

void NeighborsNode(st_Node *pNode)
{
	//------------------------------------------------------
	// �θ� �������� �ʴ´ٸ� ���� Ž�������� ���� ����̴�. 
	// 8�������� �� ��Ʈ����.
	//------------------------------------------------------
	if (pNode->pParentNode == nullptr)
	{
		int Xpos = pNode->m_iXpos;
		int Ypos = pNode->m_iYpos;

		CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
		CheckDirection(pNode, Xpos, Ypos - 1, dfDIR_UU);
		CheckDirection(pNode, Xpos + 1, Ypos - 1, dfDIR_RU);

		CheckDirection(pNode, Xpos - 1, Ypos, dfDIR_LL);
		CheckDirection(pNode, Xpos + 1, Ypos, dfDIR_RR);

		CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
		CheckDirection(pNode, Xpos, Ypos + 1, dfDIR_DD);
		CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
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

		int DirX =   parentX - Xpos;
		int DirY =   parentY - Ypos;
		/*
			dfDIR_RU �� ���  Xpos�� ����, Ypos�� ����
			dfDIR_UU �� ���  Xpos�� 0, Ypos�� ����
			dfDIR_LU �� ���  Xpos�� ��� Ypos�� ����

			dfDIR_RR �� ��� Xpos�� ����  Ypos�� 0
			dfDIR_LL �� ��� Xpos�� ���  Ypos �� 0

			dfDIR_RD �� ��� Xpos�� ���� Ypos�� ���
			dfDIR_DD �� ��� Xpos�� 0, Ypos�� ���
			dfDIR_LD �� ��� Xpos�� ���, Ypos�� ���
		*/
		
		// dfDIR_RR
		if (DirX < 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos + 1, Ypos, dfDIR_RR);
			// �ɼ�üũ RU, RD
			if(!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, dfDIR_RU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
		}
		//dfDIR_LL
		else if (DirX > 0 && DirY == 0)
		{
			CheckDirection(pNode, Xpos - 1, Ypos, dfDIR_LL);
			// �ɼ�üũ LU, LD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
		}
		// dfDIR_UU
		else if (DirX == 0 && DirY > 0)
		{
			CheckDirection(pNode, Xpos, Ypos - 1, dfDIR_UU);
			// �ɼ�üũ LU, RU
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, dfDIR_RU);
		}
		// dfDIR_DD
		else if (DirX == 0 && DirY < 0)
		{
			CheckDirection(pNode, Xpos, Ypos + 1, dfDIR_DD);
			// �ɼ�üũ LD, RD
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
		}
		//dfDIR_RU
		else if (DirX < 0 && DirY > 0)
		{

			// �⺻���� UU, RU, RR
			CheckDirection(pNode, Xpos, Ypos - 1, dfDIR_UU);
			CheckDirection(pNode, Xpos + 1, Ypos - 1 , dfDIR_RU);
			CheckDirection(pNode, Xpos + 1, Ypos, dfDIR_RR);

			// �ɼ�üũ LU, RD  
			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
			if (!CheckWalkable(Xpos, Ypos + 1))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
		}
		// dfDIR_LU
		else if (DirX > 0 && DirY > 0)
		{
	
			// �⺻���� LL, LU, UU
			CheckDirection(pNode, Xpos - 1, Ypos, dfDIR_LL);
			CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
			CheckDirection(pNode, Xpos, Ypos - 1, dfDIR_UU);

			// �ɼ�üũ LD, RU
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, dfDIR_RU);
		}
		// dfDIR_RD
		else if (DirX < 0 && DirY < 0)
		{
			
			// �⺻���� RD, RR, DD
			CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
			CheckDirection(pNode, Xpos + 1, Ypos, dfDIR_RR);
			CheckDirection(pNode, Xpos, Ypos + 1, dfDIR_DD);

			// �ɼ�üũ RU, LD 
 			if (!CheckWalkable(Xpos - 1, Ypos))
				CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos + 1, Ypos - 1, dfDIR_RU);
		}
		// dfDIR_LD
		else if (DirX > 0 && DirY < 0)
		{
			// �⺻���� LD, LL, DD
			CheckDirection(pNode, Xpos - 1, Ypos + 1, dfDIR_LD);
			CheckDirection(pNode, Xpos - 1, Ypos, dfDIR_LL);
			CheckDirection(pNode, Xpos, Ypos + 1, dfDIR_DD);

			// �ɼ�üũ LU, RD
			if (!CheckWalkable(Xpos, Ypos - 1))
				CheckDirection(pNode, Xpos - 1, Ypos - 1, dfDIR_LU);
			if (!CheckWalkable(Xpos + 1, Ypos))
				CheckDirection(pNode, Xpos + 1, Ypos + 1, dfDIR_RD);
		}

	}
}

BOOL CheckWalkable(int iX, int iY)
{
	if (iX < 0 || iX >= MAP_HEIGHT)
		return false;

	if (iY < 0 || iY >= MAP_WIDTH)
		return false;

	// ��ֹ��� �ִ��� Ȯ���Ѵ�.
	if (BlockMap[iY][iX].Type == dfOBSTACLE)
		return false;

	BlockMap[iY][iX].RColor = lR_byCheckColor;
	BlockMap[iY][iX].BColor = lB_byCheckColor;
	BlockMap[iY][iX].GColor = lG_byCheckColor;

	return true;

}
// �� Ÿ�Ͽ� ���� �Ӽ�üũ��, Jump ����� True���, ������ -> OpenList�˻�, CloseList �˻� �ؼ� ������ ��¥ ����
BOOL CheckDirection(st_Node *pParent, int iX, int iY, int iDir)
{
	if (!CheckWalkable(iX, iY))
		return false;

	int JumpX = 0, JumpY = 0;  // ��� ���� X,Y ��ǥ

	if (Jump(iX, iY, iDir, &JumpX, &JumpY))
	{
		srand((unsigned)time(NULL));

		float NextG;
		// �밢���� ���
		switch (iDir)
		{
		case dfDIR_RU:
		case dfDIR_LU:
		case dfDIR_RD:
		case dfDIR_LD:
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
		newNode->m_H = abs(endPoint->m_iXpos - JumpX) + abs(endPoint->m_iYpos - JumpY);
		newNode->m_F = newNode->m_G + newNode->m_H;

		OpenList.push_back(newNode);

		return true;
	}
	return false;
}

bool CheckAxis(int iX, int iY, float NextG, st_Node *pParent)
{
	list<st_Node *>::iterator iter;

	for (iter = OpenList.begin(); iter != OpenList.end(); ++iter)
	{
		if ((*iter)->m_iXpos  > iX || (*iter)->m_iXpos  < iX)
			continue;

		if ((*iter)->m_iYpos  > iY || (*iter)->m_iYpos < iY)
			continue;

		if (iX == (*iter)->m_iXpos && iY == (*iter)->m_iYpos)
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
	}

	for (iter = CloseList.begin(); iter != CloseList.end(); ++iter)
	{
		if ((*iter)->m_iXpos  > iX || (*iter)->m_iXpos  < iX)
			continue;

		if ((*iter)->m_iYpos > iY || (*iter)->m_iYpos  < iY)
			continue;

		if (iX == (*iter)->m_iXpos && iY == (*iter)->m_iYpos)
		{
			st_Node *pNode = (*iter);
			if (pNode->m_G > NextG)
			{
				pNode->pParentNode = pParent;
				pNode->m_G = NextG;
				pNode->m_F = pNode->m_G + pNode->m_H;
			}
			// Ŭ�����Ʈ���� �����Ѵ�.
			//CloseList.erase(iter);
			//OpenList.push_back(pNode);
			return true;
		} 
	}

	return false;

}

BOOL Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY)
{
	JumpStack.empty();
	st_Jump JumpNode = { iX, iY, iDir, iX, iY };
	int X, Y, Dir, JumpX, JumpY;
	int Xpos;
	int Ypos;
	JumpStack.push(JumpNode);
	do
	{
		JumpNode = JumpStack.top();
		JumpStack.pop();

		X = JumpNode.iX;
		Y = JumpNode.iY;
		Dir = JumpNode.iDir;
		JumpX = JumpNode.JumpX;
		JumpY = JumpNode.JumpY;

		if (!CheckWalkable(X, Y))
			return false; // ���ȴ� �����ε� �𸣰ٴ�

	  // ������ Ȯ�� �� ��ǥ �ƿ�
		if (X == endPoint->m_iXpos && Y == endPoint->m_iYpos)
		{
			*pJumpX = JumpX;
			*pJumpY = JumpY;
			return true;
		}


		printf("JumpStack Size : %d \n", JumpStack.size());

		switch (iDir)
		{
		case dfDIR_DD:
			if (!CheckWalkable(X - 1, Y) && CheckWalkable(X - 1, Y + 1) ||
				!CheckWalkable(X + 1, Y) && CheckWalkable(X + 1, Y + 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

			//return Jump(X, Y + 1, iDir, pJumpX, pJumpY);
			JumpNode = { X, Y + 1, iDir, X, Y + 1 };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_RR:
			if (!CheckWalkable(X, Y - 1) && CheckWalkable(X + 1, Y - 1) ||
				!CheckWalkable(X, Y + 1) && CheckWalkable(X + 1, Y + 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

			//return Jump(X + 1, Y, iDir, pJumpX, pJumpY);
			JumpNode = { X + 1, Y, iDir, X + 1, Y };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_UU:
			if (!CheckWalkable(X - 1, Y) && CheckWalkable(X - 1, Y - 1) ||
				!CheckWalkable(X + 1, Y) && CheckWalkable(X + 1, Y - 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}
			//return Jump(X, Y - 1, iDir, pJumpX, pJumpY);
			JumpNode = { X, Y - 1, iDir, X, Y - 1 };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_LL:
			if (!CheckWalkable(X, Y - 1) && CheckWalkable(X - 1, Y - 1) ||
				!CheckWalkable(X, Y + 1) && CheckWalkable(X - 1, Y + 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}
			//return Jump(X - 1, Y, iDir, pJumpX, pJumpY);
			JumpNode = { X - 1, Y, iDir, X - 1, Y };
			JumpStack.push(JumpNode);
			break;
			
		case dfDIR_LU:
			if (!CheckWalkable(X, Y + 1) && CheckWalkable(X - 1, Y + 1) ||
				!CheckWalkable(X + 1, Y) && CheckWalkable(X + 1, Y - 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

			//- ����/���� Jump ��� ȣ��
			//Jump �� �ڳ� �߽߰� ����ǥ �ƿ� return;
			// dfDIR_LL ���� Ž��
			 Xpos = X - 1;
			 Ypos = Y;

			while (CheckWalkable(Xpos, Ypos))
			{
				if (!CheckWalkable(Xpos, Ypos - 1) && CheckWalkable(Xpos - 1, Ypos - 1) ||
					!CheckWalkable(Xpos, Ypos + 1) && CheckWalkable(Xpos - 1, Ypos + 1) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Xpos--;
			}

			// dfDIR_UU ���� Ž��
			Xpos = X;
			Ypos = Y - 1;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos - 1, Ypos) && CheckWalkable(Xpos - 1, Ypos - 1) ||
					!CheckWalkable(Xpos + 1, Ypos) && CheckWalkable(Xpos + 1, Ypos - 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Ypos--;
			}

			// - �������� ��� Jump
			//return Jump(X - 1, Y - 1, iDir, pJumpX, pJumpY);
			JumpNode = { X - 1, Y - 1, iDir, X - 1, Y - 1 };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_RU:
			if (!CheckWalkable(X, Y + 1) && CheckWalkable(X + 1, Y + 1) ||
				!CheckWalkable(X - 1, Y) && CheckWalkable(X - 1, Y - 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

		

			// dfDIR_RR ����
			Xpos = X + 1;
			Ypos = Y;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos, Ypos - 1) && CheckWalkable(Xpos + 1, Ypos - 1) ||
					!CheckWalkable(Xpos, Ypos + 1) && CheckWalkable(Xpos + 1, Ypos + 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Xpos++;
			}

			// dfDIR_UU ����
			Xpos = X;
			Ypos = Y - 1;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos - 1, Ypos) && CheckWalkable(Xpos - 1, Ypos - 1) ||
					!CheckWalkable(Xpos + 1, Ypos) && CheckWalkable(Xpos + 1, Ypos - 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Ypos--;
			}

			// - �������� ��� Jump
			JumpNode = { X + 1, Y - 1, iDir, X + 1, Y - 1 };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_RD:
			if (!CheckWalkable(X, Y - 1) && CheckWalkable(X + 1, Y - 1) ||
				!CheckWalkable(X - 1, Y) && CheckWalkable(X - 1, Y + 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

			
			//- ����/���� Jump ��� ȣ��
			//Jump �� �ڳ� �߽߰� ����ǥ �ƿ� return;
			// RR
			Xpos = X + 1;
			Ypos = Y;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos, Ypos - 1) && CheckWalkable(Xpos + 1, Ypos - 1) ||
					!CheckWalkable(Xpos, Ypos + 1) && CheckWalkable(Xpos + 1, Ypos + 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Xpos++;
			}

			// DD
			Xpos = X ;
			Ypos = Y + 1;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos - 1, Ypos) && CheckWalkable(Xpos - 1, Ypos + 1) ||
					!CheckWalkable(Xpos + 1, Ypos) && CheckWalkable(Xpos + 1, Ypos + 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Ypos++;
			}
			// - �������� ��� Jump
			//return Jump(X + 1, Y + 1, iDir, pJumpX, pJumpY);
			JumpNode = { X + 1, Y + 1, iDir, X + 1, Y + 1 };
			JumpStack.push(JumpNode);
			break;
		case dfDIR_LD:
			if (!CheckWalkable(X, Y - 1) && CheckWalkable(X - 1, Y - 1) ||
				!CheckWalkable(X + 1, Y) && CheckWalkable(X + 1, Y + 1))
			{
				*pJumpX = JumpX;
				*pJumpY = JumpY;
				return true;
			}

			
			//- ����/���� Jump ��� ȣ��
			//Jump �� �ڳ� �߽߰� ����ǥ �ƿ� return;
			// LL
			Xpos = X - 1;
			Ypos = Y;

			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos, Ypos - 1) && CheckWalkable(Xpos - 1, Ypos - 1) ||
					!CheckWalkable(Xpos, Ypos + 1) && CheckWalkable(Xpos - 1, Ypos + 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Xpos--;
			}

			// DD
			Xpos = X;
			Ypos = Y + 1;
			while (CheckWalkable(Xpos, Ypos))
			{
				if ((!CheckWalkable(Xpos - 1, Ypos) && CheckWalkable(Xpos - 1, Ypos + 1) ||
					!CheckWalkable(Xpos + 1, Ypos) && CheckWalkable(Xpos + 1, Ypos + 1)) ||
					Xpos == endPoint->m_iXpos && Ypos == endPoint->m_iYpos)
				{
					*pJumpX = JumpX;
					*pJumpY = JumpY;
					return true;
				}
				Ypos++;
			}

			// - �������� ��� Jump
			//return Jump(X - 1, Y + 1, iDir, pJumpX, pJumpY);
			JumpNode = { X - 1, Y + 1, iDir, X - 1, Y + 1 };
			JumpStack.push(JumpNode);
			break;
		}
	}while (JumpStack.size() > 0);
	return true;
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
	isSuccess = false;

	for (int y = 0; y < MAP_WIDTH; y++)
	{
		for (int x = 0; x < MAP_HEIGHT; x++)
		{
			BlockMap[y][x].BColor = 0;
			BlockMap[y][x].GColor = 0;
			BlockMap[y][x].RColor = 0;
		}
	}
	
}

void DrawMap()
{
	PatBlt(g_hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
	int Xpos = 0;
	int Ypos = 0;

	HPEN hPen, oldPen;
	hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	oldPen = (HPEN)SelectObject(g_hDC, hPen);

	// ���� ���� �׸���.
	for (int y = 0; y <= MAP_HEIGHT; y++)
	{
		MoveToEx(g_hDC, Xpos, Ypos, NULL);
		LineTo(g_hDC, Xpos, TILE_HEIGHT * MAP_WIDTH);
		Xpos += TILE_WIDTH;
	}

	// ���� ���� �׸���.
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
	HBRUSH hRandomBrush;  
	hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	hGrayBrush = CreateSolidBrush(RGB(105, 105, 105));
	hYelloBrush = CreateSolidBrush(RGB(255, 255, 0));
	hBlueBrush = CreateSolidBrush(RGB(0, 191, 255));

	std::list<st_Node *>::iterator iter;

	// ���ع� & Ÿ�� ��ĥ
	int X = 0, Y = 0;
	for (Y = 0; Y < MAP_WIDTH; Y++)
	{
		for (X = 0; X < MAP_HEIGHT; X++)
		{
			if (BlockMap[Y][X].Type == dfOBSTACLE)
			{
				hOldBrush = (HBRUSH)SelectObject(g_hDC, hGrayBrush);
				Xpos = X * TILE_WIDTH;
				Ypos = Y * TILE_HEIGHT;
				Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
			}
			else
			{
				// Ÿ�ϻ�ĥ
				hRandomBrush = CreateSolidBrush(RGB(255 - BlockMap[Y][X].RColor, 255 - BlockMap[Y][X].GColor, 255 - BlockMap[Y][X].BColor));
				hOldBrush = (HBRUSH)SelectObject(g_hDC, hRandomBrush);
				Xpos = X * TILE_WIDTH;
				Ypos = Y * TILE_HEIGHT;
				Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
				DeleteObject(hRandomBrush);
			}
		}
	}

	DeleteObject(hGrayBrush);
	// OpenList ���
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hBlueBrush);
	for (iter = OpenList.begin(); iter != OpenList.end(); iter++)
	{
		Xpos = (*iter)->m_iXpos * TILE_WIDTH;
		Ypos = (*iter)->m_iYpos * TILE_HEIGHT;
		Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	}
	DeleteObject(hBlueBrush);

	// CloseList ���
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hYelloBrush);
	for (iter = CloseList.begin(); iter != CloseList.end(); iter++)
	{
		Xpos = (*iter)->m_iXpos * TILE_WIDTH;
		Ypos = (*iter)->m_iYpos * TILE_HEIGHT;
		Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	}
	DeleteObject(hYelloBrush);

	// ���
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hGreenBrush);
	Xpos = startPoint->m_iXpos * TILE_WIDTH;
	Ypos = startPoint->m_iYpos * TILE_HEIGHT;
	Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	DeleteObject(hGreenBrush);

	// ������
	hOldBrush = (HBRUSH)SelectObject(g_hDC, hRedBrush);
	Xpos = endPoint->m_iXpos * TILE_WIDTH;
	Ypos = endPoint->m_iYpos * TILE_HEIGHT;
	Rectangle(g_hDC, Xpos, Ypos, Xpos + TILE_WIDTH, Ypos + TILE_HEIGHT);
	DeleteObject(hRedBrush);


	DeleteObject(hPen);
	SelectObject(g_hDC, hOldBrush);
	SelectObject(g_hDC, oldPen);

	PrintPath();
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
			// �簢�� ������� ���� �簢�� ����� ���� �ߴ´�.
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
}