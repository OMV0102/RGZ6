#include <windows.h>
#include <stdio.h>
using namespace std;
#define LIB "library.dll"//��� ������������ ���������������� �������������

LPCSTR szClassName = "Window";//��� ������
LPCSTR szTitle = "������������ ������ ����";//��������� ����

HWND hwnd;//���������� ����
HWND label;

char info[256];//������, � ������� ����� ���������� ���������� ������
char msg_error[128];//������ ��� ������ ��������� ������ � MessageBox

DWORD WINAPI ThreadFunc(void*)
{
	//�������� ����� � label
	SetWindowText(label, LPCSTR(""));
	//�������� ��� ������
	strcpy_s(info, LPCSTR(""));
	strcpy_s(msg_error, LPCSTR(""));

	//��������� ������������ ����������
	HINSTANCE hinstLib = LoadLibrary(TEXT(LIB));
	//���� ������������ ���������� ������� ����������
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		//� ���������� win_height ���������� ��������� ������ ������� win_height
		ImportFunction win_height = (ImportFunction)GetProcAddress(hinstLib, "win_height");
		//� ���������� support_sse ���������� ��������� ������ ������� support_sse
		ImportFunction support_sse = (ImportFunction)GetProcAddress(hinstLib, "support_sse");

		//���� ������� win_height � support_sse ������� �����-�� ��������
		if (win_height != NULL && support_sse != NULL)
		{
			//���� ������� support_sse ������� �������� 1, ������ SSE ��������������
			if (support_sse() == 1)
				sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE) supported", win_height());
			//����� �� ��������������
			else
				sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE) NOT supported", win_height());
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static
		}
		//���� ������� support_sse ������� �������� NULL
		else if (win_height != NULL && support_sse == NULL)
		{
			sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE): UNKNOWN", win_height());
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ��������� SSE!");
		}
		//���� ������� win_height ������� �������� NULL
		else if (win_height == NULL && support_sse != NULL) 
		{
			//���� ������� support_sse ������� �������� 1, ������ SSE ��������������
			if (support_sse() == 1)
				sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE) supported");
			//����� �� ��������������
			else
				sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE) NOT supported");
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ������������ ������ �������������� ����!");
		}
		//���� ��� ������� ������� �������� NULL
		else
		{
			sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE): UNKNOWN");
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ������������ ������ �������������� ����!\n�� ������� ���������� ��������� SSE!");
		}

		//����������� ���������� � ��������� ������������ ����������
		FreeLibrary(hinstLib);

		//���� ���� �����-�� �� ���� ������� (��� ���) �������(�) ������������ ��������, ������� MessageBox � �������
		if (win_height == NULL || support_sse == NULL)
			MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("������"), MB_OK | MB_ICONERROR);

		
	}
	//���� ���������� �����������
	else
	{
		//��������� ��������� �� ������ ��� label
		sprintf_s(info, "\n\n\n\n                 *** ERROR ***");//���������� � ��������� ���� static
		SetWindowText(label, LPCSTR(info));

		//��������� ��������� �� ������ ��� MessageBox
		sprintf_s(msg_error, "���������� %s �����������!\n��������� ���������� � ����� � ����������\n� ������� ������ \"��������\".", LIB);
		MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("������"), MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread;
	DWORD IDThread;
	switch (msg)
	{	
		case WM_CTLCOLORSTATIC:
		{
			DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
			if (CtrlID == 1001)
			{
				SetTextColor((HDC)wParam, RGB(255, 255, 255));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
			}
		}

		case WM_COMMAND:
		{
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			CloseHandle(hThread);
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
	//====================================
	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc = WindowFunc;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = hThisInst;
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = szClassName;
	RegisterClass(&wcl);
	//====================================

	hwnd = CreateWindow(
		szClassName,
		szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		700,
		400,
		400, 185,
		NULL, NULL, hThisInst, NULL);
	//====================================
	label = CreateWindow(
		"static",
		LPCSTR(""),
		WS_CHILD | WS_VISIBLE,
		5, 5,
		373, 100,
		hwnd,
		(HMENU)1001,
		hThisInst, NULL);
	//====================================
	SendDlgItemMessage(hwnd, 1001, WM_SETFONT, (WPARAM)font_mono, TRUE);

	HANDLE hThread;
	DWORD IDThread;
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread);

	CreateWindow(
		"button",
		"��������",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		150, 110,
		85, 30,
		hwnd,
		(HMENU)1003,
		hThisInst, NULL);

	ShowWindow(hwnd, nWinMode);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
