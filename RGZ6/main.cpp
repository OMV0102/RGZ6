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
				sprintf_s(info, "������������ ������ �������������� ����: %d\n\n��������� SIMD-���������� ���������� (SSE): ��������������", win_height());
			//����� �� ��������������
			else
				sprintf_s(info, "������������ ������ �������������� ����: %d\n\n��������� SIMD-���������� ���������� (SSE): �� ��������������", win_height());
			SetWindowText(label, LPCSTR(info));
		}
		//���� ������� support_sse ������� �������� NULL
		else if (win_height != NULL && support_sse == NULL)
		{
			sprintf_s(info, "������������ ������ �������������� ����: %d\n\n��������� SIMD-���������� ���������� (SSE): ����������", win_height());
			SetWindowText(label, LPCSTR(info));

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ��������� SSE!\n�������� �� ����������� �� ������������ ���� %s.", LIB);
		}
		//���� ������� win_height ������� �������� NULL
		else if (win_height == NULL && support_sse != NULL) 
		{
			//���� ������� support_sse ������� �������� 1, ������ SSE ��������������
			if (support_sse() == 1)
				sprintf_s(info, "������������ ������ �������������� ����: ����������\n\n��������� SIMD-���������� ���������� (SSE): ��������������");
			//����� �� ��������������
			else
				sprintf_s(info, "������������ ������ �������������� ����: ����������\n\n��������� SIMD-���������� ���������� (SSE): �� ��������������");
			SetWindowText(label, LPCSTR(info));

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ������������ ������ �������������� ����!\n�������� �� ����������� �� ������������ ���� %s.", LIB);
		}
		//���� ��� ������� ������� �������� NULL
		else
		{
			sprintf_s(info, "������������ ������ �������������� ����: ����������\n\n��������� SIMD-���������� ���������� (SSE): ����������");
			SetWindowText(label, LPCSTR(info));

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ������������ ������ �������������� ����!\n�� ������� ���������� ��������� SSE!\n�������� �� ����������� �� ������������ ���� %s.", LIB);
		}

		//���� [���� �����-�� �� ���� ������� (��� ���) ������� ������������ ��������, ������� MessageBox � �������
		if (win_height == NULL || support_sse == NULL)
			MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("������"), MB_OK | MB_ICONERROR);

		//����������� ���������� � ��������� ������������ ����������
		FreeLibrary(hinstLib);
	}
	//���� ���������� �����������
	else
	{
		//��������� ��������� �� ������ ��� label
		sprintf_s(info, "���������� %s �����������!\n��������� ���������� � ����� � ����������\n� ������� ������ \"��������\".", LIB);
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
				SetTextColor((HDC)wParam, RGB(200, 200, 200));
				SetBkColor((HDC)wParam, RGB(0, 0, 0));
				return (INT_PTR)GetStockObject(BLACK_BRUSH);
			}
		}

		case WM_COMMAND: /** Button clicked */
		{
			if (LOWORD(wParam) == 1003)
			{
				/** Launch in new thread */
				hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
				CloseHandle(hThread);
			}
			break;
		}

		case WM_DESTROY: /** Window closed */ 
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
	HFONT font_std = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);

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

	/** Some main window */
	int window_width = 300;
	int window_height = 200;
	int button_width = 75;
	int button_height = 25;
	int border = 5;
	HDC hDCScreen = GetDC(NULL);
	hwnd = CreateWindow(szClassName, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		(GetDeviceCaps(hDCScreen, HORZRES) - window_width) / 2, (GetDeviceCaps(hDCScreen, VERTRES) - window_height) / 2,
		window_width, window_height, NULL, NULL, hThisInst, NULL);
	RECT rt;
	GetClientRect(hwnd, &rt);
	window_width = rt.right;
	window_height = rt.bottom;
	label = CreateWindow("static", LPCSTR("������� \"��������\"!"), WS_CHILD | WS_VISIBLE,
		border, border, window_width - 2 * border, window_height - 2*button_height,
		hwnd, (HMENU)1001, hThisInst, NULL);
	SendDlgItemMessage(hwnd, 1001, WM_SETFONT, (WPARAM)font_mono, TRUE);

	HANDLE hThread;
	DWORD IDThread;
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	CloseHandle(hThread);

	CreateWindow("button", "��������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height,
		hwnd, (HMENU)1003, hThisInst, NULL);

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
