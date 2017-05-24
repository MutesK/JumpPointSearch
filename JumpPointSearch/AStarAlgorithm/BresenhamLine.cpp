#include "stdafx.h"
#include "BresenhamLine.h"


CBresenhamLine::CBresenhamLine(int iStartX, int iStartY, int iEndX, int iEndY)
{
	SetPosition(iStartX, iStartY, iEndX, iEndY);
}


CBresenhamLine::~CBresenhamLine()
{
}


void CBresenhamLine::SetPosition(int iStartX, int iStartY, int iEndX, int iEndY)
{
	iCurX = iStartX;
	iCurY = iStartY;
	this->iEndX = iEndX;
	this->iEndY = iEndY;
	m_iError = 0;


	dX = abs(iStartX - iEndX);
	dY = abs(iStartY - iEndY);
	if (dX == 0)
		incrementX = 0;
	else 
		incrementX = (iEndX - iStartX) / dX;
	if (dY == 0)
		incrementY = 0;
	else 
		incrementY = (iEndY - iStartY) / dY;
}

bool CBresenhamLine::GetNext(int *pNextX, int *pNextY)
{
	if (iCurX == iEndX && iCurY == iEndY)
		return true;

	if (dX >= dY)
	{
		iCurX += incrementX;
		m_iError += dY;

		if (m_iError >= dX / 2)
		{
			iCurY += incrementY;
			m_iError -= dX;
		}

		*pNextX = iCurX;
		*pNextY = iCurY;
		return false;
	}
	else
	{
		iCurY += incrementY;
		m_iError += dX;

		if (m_iError >= dY / 2)
		{
			iCurX += incrementX;
			m_iError -= dY;
		}

		*pNextX = iCurX;
		*pNextY = iCurY;
		return false;
	}

}

bool CBresenhamLine::PeekNext(int *pNextX, int *pNextY)
{

	int CurX = iCurX, CurY = iCurY;
	int _dX = dX, _dY = dY;
	int _Error = m_iError;

	if (CurX == iEndX && CurY == iEndY)
		return true;

	if (_dX >= _dY)
	{
		CurX += incrementX;
		_Error += _dY;

		if (_Error >= _dX / 2)
		{
			CurY += incrementY;
			_Error -= _dX;
		}

		*pNextX = CurX;
		*pNextY = CurY;
		return false;
	}
	else
	{
		CurY += incrementY;
		_Error += _dX;

		if (_Error >= _dY / 2)
		{
			CurX += incrementX;
			_Error -= _dY;
		}

		*pNextX = CurX;
		*pNextY = CurY;
		return false;
	}
}