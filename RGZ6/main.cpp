#include <windows.h>
#include <stdio.h>
using namespace std;
#define LIB "library.dll"//имя подключаемой скомпилированной библиблиотеки

LPCSTR szClassName = "Window";//имя класса
LPCSTR szTitle = "Максимальная высота окна";//заголовок окна

HWND hwnd;//дескриптор окна
HWND label;

char info[256];//строка, в которую будем запоминать полученные данные
char msg_error[128];//строка для вывода возможной ошибки в MessageBox

DWORD WINAPI ThreadFunc(void*)
{
	//обнуляем текст в label
	SetWindowText(label, LPCSTR(""));
	//обнуляем все строки
	strcpy_s(info, LPCSTR(""));
	strcpy_s(msg_error, LPCSTR(""));

	//загружаем динамическую библиотеку
	HINSTANCE hinstLib = LoadLibrary(TEXT(LIB));
	//если динамическая библиотека успешно загруженна
	if (hinstLib != NULL)
	{
		typedef int(*ImportFunction)();
		//в переменную win_height запоминаем результат работы функции win_height
		ImportFunction win_height = (ImportFunction)GetProcAddress(hinstLib, "win_height");
		//в переменную support_sse запоминаем результат работы функции support_sse
		ImportFunction support_sse = (ImportFunction)GetProcAddress(hinstLib, "support_sse");

		//если функции win_height и support_sse вернули какое-то значение
		if (win_height != NULL && support_sse != NULL)
		{
			//если функция support_sse вернула значение 1, значит SSE поддерживается
			if (support_sse() == 1)
				sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE) supported", win_height());
			//иначе не поддерживается
			else
				sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE) NOT supported", win_height());
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static
		}
		//если функция support_sse вернула значение NULL
		else if (win_height != NULL && support_sse == NULL)
		{
			sprintf_s(info, "\n\n Maximum height of full screen window: %d\n\n\n Streaming SIMD Extensions (SSE): UNKNOWN", win_height());
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static

			//формируем сообщение об ошибке для MessageBox
			sprintf_s(msg_error, "Не удалось определить поддержку SSE!");
		}
		//если функция win_height вернула значение NULL
		else if (win_height == NULL && support_sse != NULL) 
		{
			//если функция support_sse вернула значение 1, значит SSE поддерживается
			if (support_sse() == 1)
				sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE) supported");
			//иначе не поддерживается
			else
				sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE) NOT supported");
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static

			//формируем сообщение об ошибке для MessageBox
			sprintf_s(msg_error, "Не удалось определить максимальную высоту полноэкранного окна!");
		}
		//если обе функции вернули значение NULL
		else
		{
			sprintf_s(info, "\n\n Maximum height of full screen window: UNKNOWN\n\n\n Streaming SIMD Extensions (SSE): UNKNOWN");
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static

			//формируем сообщение об ошибке для MessageBox
			sprintf_s(msg_error, "Не удалось определить максимальную высоту полноэкранного окна!\nНе удалось определить поддержку SSE!");
		}

		//освобождаем дескриптор и закрываем динамическую библиотеку
		FreeLibrary(hinstLib);

		//если хоть какая-то из двух функций (или обе) вернула(и) некорректное значение, выводим MessageBox с ошибкой
		if (win_height == NULL || support_sse == NULL)
			MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("Ошибка"), MB_OK | MB_ICONERROR);

		
	}
	//если библиотека отсутствует
	else
	{
		//формируем сообщение об ошибке для label
		sprintf_s(info, "\n\n\n\n                 *** ERROR ***");//записываем в текстовое поле static
		SetWindowText(label, LPCSTR(info));

		//формируем сообщение об ошибке для MessageBox
		sprintf_s(msg_error, "Библиотека %s отсутствует!\nПоместите библиотеку в папку с программой\nи нажмите кнопку \"Обновить\".", LIB);
		MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("Ошибка"), MB_OK | MB_ICONERROR);
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
		"Обновить",
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
