#pragma once
class CBresenhamLine
{
public:
	CBresenhamLine(){}
	CBresenhamLine(int iStartX, int iStartY, int iEndX, int iEndY);
	~CBresenhamLine();


	void SetPosition(int iStartX, int iStartY, int iEndX, int iEndY);
	bool GetNext(int *pNextX, int *pNextY);
	bool PeekNext(int *pNextX, int *pNextY);

private:
	int m_iError;
	int iCurX, iCurY;
	int iEndX, iEndY;
	int incrementX, incrementY;
	int dX, dY;
};

