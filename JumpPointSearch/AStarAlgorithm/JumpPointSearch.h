#pragma once
#include "stdafx.h"
#include <algorithm>
#include <set>

using namespace std;


class CJumpPointSearch
{
public:
	struct st_Node
	{
		// Ÿ�� ��ġ
		int m_iXpos;
		int m_iYpos;

		st_Node *pParentNode;
		st_Node *pNext; // ��������Ʈ ��ġ�� �츮�� ���ϴ� ������ �̸� ���������� �ٲٱ� ���� ������

		float m_F;
		float m_G;
		float m_H;
	};

	// �̵�����
	enum
	{
		enDIR_RU  = 10,
		enDIR_UU,
		enDIR_LU,
		enDIR_RR,
		enDIR_LL,
		enDIR_RD,
		enDIR_DD,
		enDIR_LD
	};


	// ��¿� X,Y �� ����ü
	struct st_Point
	{
		int X;
		int Y;
	};

public:
	// ���� ũ��� ���� ������(Ÿ��), OpenList �Ѱ��ġ, Jump�Ѱ��ġ
	CJumpPointSearch(int m_iWidth, int m_iHeight, int m_MaxOpen = 65536, int MaxJump = 65536);
	~CJumpPointSearch();

	// ���� ��ǥ, �� ��ǥ, ��¿� ��ǥ�迭, �迭 ũ��
	// Returns : True : �� ã�� ����
	//			 False : �� ã�� ����
	bool PathFind(int startX, int startY, int DestX, int DestY, st_Point *pArray, int ArraySize);
	bool MapOpen(st_Tile (*pMap));
private:
	template <typename T>
	struct NODE_COMPARE
	{
		bool operator() (T NodeA, T NodeB) const
		{
			return NodeA->m_F < NodeB->m_F;
		}
	};

	struct NODE_FINDER
	{
		NODE_FINDER(int iX, int iY) : iPosX(iX), iPosY(iY)
		{
		}
		bool operator() (CJumpPointSearch::st_Node *pNode) const
		{
			if (pNode->m_iXpos == iPosX && pNode->m_iYpos == iPosY)
				return true;

			return false;
		}
		int iPosX;
		int iPosY;
	};

	BOOL Jump(int iX, int iY, int iDir, int *pJumpX, int *pJumpY);

	// Jump �Լ�
	bool Jump_DD(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_RR(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_UU(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_LL(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_LU(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_RU(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_RD(int iX, int iY, int *pJumpX, int *pJumpY);
	bool Jump_LD(int iX, int iY, int *pJumpX, int *pJumpY);

	// Returns: True : ���¸���Ʈ�� Ŭ�����Ʈ�� �� ������ �߻�
	//			False : �ƹ��͵� ���ŵ��� ����.
	bool CheckAxis(int iX, int iY, float NextG, st_Node *pParent);
	BOOL CheckDirection(st_Node *pParent, int iX, int iY, int iDir);
	void NeighborsNode(st_Node *pNode);

	void InitNode(int iStartX, int iStartY, int iEndX, int iEndY);
	void InputArray(st_Point *pArray, int ArraySize);

	void Clear();



private:
	//  ���� ũ��
	unsigned m_iWidth;
	unsigned m_iHeight;

	unsigned m_iMaxOpen;
	unsigned m_iOpenCount;

	unsigned m_iMaxJump;
	unsigned m_iJumpCount;


	st_Node* m_pStartPoint;
	st_Node* m_pEndPoint;

	multiset<st_Node *, NODE_COMPARE<st_Node *>> OpenList;
	multiset<st_Node *, NODE_COMPARE<st_Node *>> CloseList;
	
	st_Tile *p_BlockMap;

#define CheckWalkable(X, Y)  !(((X) < 0) || ((Y) < 0) || ((X) >= m_iHeight) || ((Y) >= m_iWidth) || ((p_BlockMap + (Y) * m_iHeight + (X))->Type == dfOBSTACLE))
};


