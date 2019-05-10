#include <windows.h>
#include <stdio.h>
using namespace std;
#define LIB "library.dll"//��� ������������ ���������������� �������������

LPCSTR szClassName = "Window";//��� ������
LPCSTR szTitle = "������������ ������ ���� / SSE";//��������� ����

HWND hwnd;//���������� ����
HWND label;//���������� �������� static (��������� ����)

char info[256];//������, � ������� ����� ���������� ���������� ������
char msg_error[128];//������ ��� ������ ��������� ������ � MessageBox

DWORD WINAPI ThreadFunc(void*)
{
	//�������� ����� � label
	SetWindowText(label, "");
	//�������� ��� ������
	strcpy_s(info, "");
	strcpy_s(msg_error, "");

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
				sprintf_s(info, "\n  Maximum height of full screen window:  %d\n\n\n Streaming SIMD Extensions (SSE) supported", win_height());
			//����� �� ��������������
			else
				sprintf_s(info, "\n  Maximum height of full screen window: %d\n\n\n  Streaming SIMD Extensions (SSE) NOT supported", win_height());
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static
		}
		//���� ������� support_sse ������� �������� NULL
		else if (win_height != NULL && support_sse == NULL)
		{
			sprintf_s(info, "\n  Maximum height of full screen window: %d\n\n\n  Streaming SIMD Extensions (SSE): UNKNOWN", win_height());
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ��������� SSE!");
		}
		//���� ������� win_height ������� �������� NULL
		else if (win_height == NULL && support_sse != NULL) 
		{
			//���� ������� support_sse ������� �������� 1, ������ SSE ��������������
			if (support_sse() == 1)
				sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE) supported");
			//����� �� ��������������
			else
				sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE) NOT supported");
			SetWindowText(label, LPCSTR(info));//���������� � ��������� ���� static

			//��������� ��������� �� ������ ��� MessageBox
			sprintf_s(msg_error, "�� ������� ���������� ������������ ������ �������������� ����!");
		}
		//���� ��� ������� ������� �������� NULL
		else
		{
			sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE): UNKNOWN");
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
		sprintf_s(info, "\n\n\n\t\t      *** ERROR ***");//���������� � ��������� ���� static
		SetWindowText(label, LPCSTR(info));

		//��������� ��������� �� ������ ��� MessageBox
		sprintf_s(msg_error, "���������� %s �����������!\n��������� ���������� � ����� � ����������\n� ������� ������ \"��������\".", LIB);
		MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("������"), MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread; // ���������� ������
	DWORD IDThread; // ������������� ������
	switch (msg)
	{	
		//��������� ������ ���� � ������ � �������� static (��������� ����)
		case WM_CTLCOLORSTATIC:
		{
			//���� ������
			SetTextColor((HDC)wParam, RGB(255, 255, 255));
			//���� ����
			SetBkColor((HDC)wParam, RGB(0, 0, 0));
			return (INT_PTR)GetStockObject(BLACK_BRUSH);//������ �����
		}

		//��������� ������� ������ ��������
		case WM_COMMAND:
		{
			//�������� ������, � ������ �������� ������� ThreadFunc
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			//��������� �����, ������� ����������
			CloseHandle(hThread);
			break;
		}

		//�������� ����������
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		//��������� ��������� �� ���������
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;//���������
	HANDLE hThread; // ���������� ������
	DWORD IDThread; // ������������� ������
	//====================================
	//��������� ������ ����
	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;//����� ���� (�������� ����������� ����)
	wcl.lpfnWndProc = WindowFunc;//��������� �� ���������
	wcl.cbClsExtra = 0;//����� �������������� ���� ����� ��������� ������ ����
	wcl.cbWndExtra = 0;//����� �������������� ���� ����� ���������� ����
	wcl.hInstance = hThisInst;//���������� ����������, ������� �������� ������� ��������� ��� ������
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);//������� ����������
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);//������������� ������ �� ���������
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//����� ��� ����
	wcl.lpszMenuName = NULL;//���� ������ �����������
	wcl.lpszClassName = szClassName;//��� ������
	RegisterClass(&wcl);
	//====================================
	//�������� ��������� ����
	hwnd = CreateWindow(
		szClassName,//��� ������
		szTitle,//��������� ����
		//�����, ���������� �� ����� ����:
		//������������� ����, ��������� ����, ���� ����
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		700, 400,//�������������� � ������������ ��������� ���� �� ������
		377, 185,//������ � ������ ����
		//���������� ������������� ���� � ���������� ����������
		HWND_DESKTOP, NULL, hThisInst, NULL);
	//====================================
	//�������� ���������� ����
	label = CreateWindow(
		"static",//��� ������
		"",//��������� �����
		//�����, ���������� �� ����� ���������� ����:
		//�������� ����, ������������� ������
		WS_CHILD | WS_VISIBLE,
		5, 5,//�������������� � ������������ ��������� ���������� ���� � ����
		350, 100,//������ � ������ ���������� ����
		//���������� ������������� ���� � ���������� ����������
		hwnd, NULL, hThisInst, NULL);
	//====================================
	//�������� ������, � ������� ���������� ������� ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	//��������� �����, ������� ����������
	CloseHandle(hThread);
	//====================================
	//�������� ������ "��������"
	CreateWindow(
		"button",//��� ������
		"��������",//��������� �����
		//�����, ���������� �� ����� ������:
		//�������� ����, ������������� ������, ��������� ������� � WM_COMMAND
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		145, 110,//�������������� � ������������ ��������� ������ � ����
		85, 30,//������ � ������ ������
		//���������� ������������� ���� � ���������� ����������
		hwnd, NULL, hThisInst, NULL);
	//====================================
	ShowWindow(hwnd, nWinMode);//����� ���� �� ������
	//====================================
	//��������� ����������� ���������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
