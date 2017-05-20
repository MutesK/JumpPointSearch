// AStar.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "AStarAlgorithm.h"
#include <list>
#include "JPS.h"
#include "resource.h"
#include "JumpPointSearch.h"
using namespace std;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

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

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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
	// TODO: 여기에 코드를 입력합니다.
	if (AllocConsole()) {
		freopen("CONIN$", "rb", stdin);
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
	}



	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_ASTARALGORITHM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
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


	// 기본 메시지 루프입니다.
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
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
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
		// 메뉴 선택을 구문 분석합니다.
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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.

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
		// 시작점 설정
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
		// 목적지 설정
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
		// 길찾기 시작
		// 타이머 동작시작
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
				// PathFind 완료거나 못찾음.

				Search.MapOpen(BlockMap[0]);
				memset(Point, 0, sizeof(CJumpPointSearch::st_Point) * 100);
				Search.PathFind(startPoint->m_iXpos, startPoint->m_iYpos, endPoint->m_iXpos,
					endPoint->m_iYpos, Point, 100);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		isDown = true;

		// 해당 방해물 삭제

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
			// 방해물 설정
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
	// 기초 타일 그리기
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
	HBRUSH hRandomBrush;
	hGreenBrush = CreateSolidBrush(RGB(0, 255, 0));
	hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
	hGrayBrush = CreateSolidBrush(RGB(105, 105, 105));
	hYelloBrush = CreateSolidBrush(RGB(255, 255, 0));
	hBlueBrush = CreateSolidBrush(RGB(0, 191, 255));

	std::list<st_Node *>::iterator iter;

	// 방해물 & 타일 색칠
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
				// 타일색칠
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
