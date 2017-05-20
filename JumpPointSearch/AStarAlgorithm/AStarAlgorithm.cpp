// AStar.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "AStarAlgorithm.h"
#include <list>
#include "JPS.h"
#include "resource.h"
#include "JumpPointSearch.h"
using namespace std;

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

bool isLDown = false;
bool isFirst = true;
bool isSuccess = false;
HBITMAP g_hMemDC_Bitmap;
HDC g_hDC;
HBITMAP g_hMemDC_OldBitmap;
RECT rect;
HWND g_hWnd; HANDLE hTimer;

st_Node* startPoint;
st_Node* endPoint;
st_Tile BlockMap[MAP_WIDTH][MAP_HEIGHT];
CJumpPointSearch::st_Point Point[100];

list<st_Node *> OpenList;
list<st_Node *> CloseList;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawMap();
void Render();

CJumpPointSearch Search(MAP_WIDTH, MAP_HEIGHT);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	if (AllocConsole()) {
		freopen("CONIN$", "rb", stdin);
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
	}



	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ASTARALGORITHM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTARALGORITHM));

	MSG msg;

	startPoint = new st_Node;
	endPoint = new st_Node;

	startPoint->m_iXpos = 10;
	startPoint->m_iYpos = 10;

	endPoint->m_iXpos = 30;
	endPoint->m_iYpos = 30;


	// �⺻ �޽��� �����Դϴ�.
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render
			Render();
		}
	}

	return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTARALGORITHM));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ASTARALGORITHM);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hWnd = hWnd;

	return TRUE;

}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool isDown = false;
	static bool timer = false;
	int Xpos;
	int Ypos;
	switch (message)
	{
	case WM_CREATE:
	{
		GetClientRect(hWnd, &rect);

		HDC hdc = GetDC(hWnd);

		g_hDC = CreateCompatibleDC(hdc);
		g_hMemDC_Bitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		g_hMemDC_OldBitmap = (HBITMAP)SelectObject(g_hDC, g_hMemDC_Bitmap);

		ReleaseDC(hWnd, hdc);

		PatBlt(g_hDC, 0, 0, rect.right, rect.bottom, WHITENESS);
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case ID_REMOVEWALL:
			memset(BlockMap, 0, sizeof(st_Tile) * MAP_WIDTH * MAP_HEIGHT);
			break;
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.

		BitBlt(hdc, 0, 0, rect.right, rect.bottom, g_hDC, 0, 0, SRCCOPY);

		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		SelectObject(g_hDC, g_hMemDC_OldBitmap);
		DeleteObject(g_hMemDC_Bitmap);
		DeleteDC(g_hDC);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDBLCLK:
		// ������ ����
		Xpos = LOWORD(lParam) / TILE_WIDTH;
		Ypos = HIWORD(lParam) / TILE_HEIGHT;

		if (Xpos < 0 || Xpos >= MAP_HEIGHT)
			return DefWindowProc(hWnd, message, wParam, lParam);

		if (Ypos < 0 || Ypos >= MAP_WIDTH)
			return DefWindowProc(hWnd, message, wParam, lParam);

		startPoint->m_iXpos = Xpos;
		startPoint->m_iYpos = Ypos;
		break;
	case WM_RBUTTONDBLCLK:
		// ������ ����
		Xpos = LOWORD(lParam) / TILE_WIDTH;
		Ypos = HIWORD(lParam) / TILE_HEIGHT;

		if (Xpos < 0 || Xpos >= MAP_HEIGHT)
			return DefWindowProc(hWnd, message, wParam, lParam);

		if (Ypos < 0 || Ypos >= MAP_WIDTH)
			return DefWindowProc(hWnd, message, wParam, lParam);

		endPoint->m_iXpos = Xpos;
		endPoint->m_iYpos = Ypos;
		break;
	case WM_RBUTTONDOWN:
		// ��ã�� ����
		// Ÿ�̸� ���۽���
		Clear();
		
		hTimer = (HANDLE)SetTimer(hWnd, 1, 100, NULL);
		SendMessage(hWnd, WM_TIMER, 1, 0);
		timer = true;
		break;

	case WM_TIMER:
		if (timer)
		{
			if (JumpPointSearch(startPoint->m_iXpos, startPoint->m_iYpos,
				endPoint->m_iXpos, endPoint->m_iYpos, true))
			{
				KillTimer(hWnd, 1);
				timer = false;
				// PathFind �Ϸ�ų� ��ã��.

				Search.MapOpen(BlockMap[0]);
				memset(Point, 0, sizeof(CJumpPointSearch::st_Point) * 100);
				Search.PathFind(startPoint->m_iXpos, startPoint->m_iYpos, endPoint->m_iXpos,
					endPoint->m_iYpos, Point, 100);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		isDown = true;

		// �ش� ���ع� ����

		Xpos = LOWORD(lParam) / TILE_WIDTH;
		Ypos = HIWORD(lParam) / TILE_HEIGHT;

		if (Xpos < 0 || Xpos >= MAP_HEIGHT)
			return DefWindowProc(hWnd, message, wParam, lParam);

		if (Ypos < 0 || Ypos >= MAP_WIDTH)
			return DefWindowProc(hWnd, message, wParam, lParam);

		if (BlockMap[Ypos][Xpos].Type == dfOBSTACLE)
			BlockMap[Ypos][Xpos].Type = dfEMPTY;
		
		break;
	case WM_LBUTTONUP:
		isDown = false;
		break;
	case WM_MOUSEMOVE:
		if (isDown)
		{
			// ���ع� ����
			Xpos = LOWORD(lParam);
			Ypos = HIWORD(lParam);

			st_Tile * newPoint = new st_Tile;

			int m_Xpos = Xpos / TILE_WIDTH;
			int m_Ypos = Ypos / TILE_HEIGHT;

			if (m_Xpos< 0 || m_Xpos >=  MAP_HEIGHT)
				return DefWindowProc(hWnd, message, wParam, lParam);

			if (m_Ypos < 0 || m_Ypos >=  MAP_WIDTH)
				return DefWindowProc(hWnd, message, wParam, lParam);

			if (BlockMap[m_Ypos][m_Xpos].Type == dfOBSTACLE)
				break;

			BlockMap[m_Ypos][m_Xpos].Type = dfOBSTACLE;

		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void Render()
{
	// ���� Ÿ�� �׸���
	DrawMap();

	InvalidateRect(g_hWnd, NULL, FALSE);
	Sleep(0);
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

		hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		oldPen = (HPEN)SelectObject(g_hDC, hPen);


		for (int i = 1; i < 100; i++)
		{
			if (Point[i].X == 0 && Point[i].Y == 0)
				return;

			int StartXpos = Point[i - 1].X * TILE_WIDTH + TILE_WIDTH / 2;
			int StartYpos = Point[i - 1].Y * TILE_HEIGHT + TILE_HEIGHT / 2;
			int EndXpos = Point[i].X * TILE_WIDTH + TILE_WIDTH / 2;
			int EndYpos = Point[i].Y * TILE_HEIGHT + TILE_HEIGHT / 2;

			MoveToEx(g_hDC, StartXpos, StartYpos, NULL);
			LineTo(g_hDC, EndXpos, EndYpos);
		}

		DeleteObject(hPen);
		SelectObject(g_hDC, oldPen);
	}

}
