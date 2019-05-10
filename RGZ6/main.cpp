#include <windows.h>
#include <stdio.h>
using namespace std;
#define LIB "library.dll"//имя подключаемой скомпилированной библиблиотеки

LPCSTR szClassName = "Window";//имя класса
LPCSTR szTitle = "Максимальная высота окна / SSE";//заголовок окна

HWND hwnd;//дескриптор окна
HWND label;//дескриптор элемента static (текстовое поле)

char info[256];//строка, в которую будем запоминать полученные данные
char msg_error[128];//строка для вывода возможной ошибки в MessageBox

DWORD WINAPI ThreadFunc(void*)
{
	//обнуляем текст в label
	SetWindowText(label, "");
	//обнуляем все строки
	strcpy_s(info, "");
	strcpy_s(msg_error, "");

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
				sprintf_s(info, "\n  Maximum height of full screen window:  %d\n\n\n Streaming SIMD Extensions (SSE) supported", win_height());
			//иначе не поддерживается
			else
				sprintf_s(info, "\n  Maximum height of full screen window: %d\n\n\n  Streaming SIMD Extensions (SSE) NOT supported", win_height());
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static
		}
		//если функция support_sse вернула значение NULL
		else if (win_height != NULL && support_sse == NULL)
		{
			sprintf_s(info, "\n  Maximum height of full screen window: %d\n\n\n  Streaming SIMD Extensions (SSE): UNKNOWN", win_height());
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static

			//формируем сообщение об ошибке для MessageBox
			sprintf_s(msg_error, "Не удалось определить поддержку SSE!");
		}
		//если функция win_height вернула значение NULL
		else if (win_height == NULL && support_sse != NULL) 
		{
			//если функция support_sse вернула значение 1, значит SSE поддерживается
			if (support_sse() == 1)
				sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE) supported");
			//иначе не поддерживается
			else
				sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE) NOT supported");
			SetWindowText(label, LPCSTR(info));//записываем в текстовое поле static

			//формируем сообщение об ошибке для MessageBox
			sprintf_s(msg_error, "Не удалось определить максимальную высоту полноэкранного окна!");
		}
		//если обе функции вернули значение NULL
		else
		{
			sprintf_s(info, "\n  Maximum height of full screen window: UNKNOWN\n\n\n  Streaming SIMD Extensions (SSE): UNKNOWN");
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
		sprintf_s(info, "\n\n\n\t\t      *** ERROR ***");//записываем в текстовое поле static
		SetWindowText(label, LPCSTR(info));

		//формируем сообщение об ошибке для MessageBox
		sprintf_s(msg_error, "Библиотека %s отсутствует!\nПоместите библиотеку в папку с программой\nи нажмите кнопку \"Обновить\".", LIB);
		MessageBox(hwnd, LPCSTR(msg_error), LPCSTR("Ошибка"), MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread; // дескриптор потока
	DWORD IDThread; // идентификатор потока
	switch (msg)
	{	
		//установка цветов фона и текста у элемента static (текстовое поле)
		case WM_CTLCOLORSTATIC:
		{
			//цвет текста
			SetTextColor((HDC)wParam, RGB(255, 255, 255));
			//цвет фона
			SetBkColor((HDC)wParam, RGB(0, 0, 0));
			return (INT_PTR)GetStockObject(BLACK_BRUSH);//черная кисть
		}

		//обработка нажатия кнопки Обновить
		case WM_COMMAND:
		{
			//создание потока, в котром вызывает функция ThreadFunc
			hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
			//закрываем поток, удаляем дескриптор
			CloseHandle(hThread);
			break;
		}

		//закрытие приложение
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		//обработка сообщения по умолчанию
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
	MSG msg;//сообщение
	HANDLE hThread; // дескриптор потока
	DWORD IDThread; // идентификатор потока
	//====================================
	//параметры класса окна
	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW;//стиль окна (включает перерисовку окна)
	wcl.lpfnWndProc = WindowFunc;//указатель на процедуру
	wcl.cbClsExtra = 0;//число дополнительных байт после структуры класса окна
	wcl.cbWndExtra = 0;//число дополнительных байт после экземпляра окна
	wcl.hInstance = hThisInst;//дескриптор экземпляра, который содержит оконную процедуру для класса
	wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);//логотип приложения
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW);//устанавливаем курсор по умолчанию
	wcl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//белый фон окна
	wcl.lpszMenuName = NULL;//меню класса отсутствует
	wcl.lpszClassName = szClassName;//имя класса
	RegisterClass(&wcl);
	//====================================
	//создание основного окна
	hwnd = CreateWindow(
		szClassName,//имя класса
		szTitle,//заголовок окна
		//флаги, отвечающие за стиль окна:
		//перекрывающее окно, заголовок окна, меню окна
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		700, 400,//горизонтальное и вертикальное положение окна на экране
		377, 185,//ширина и высота окна
		//дескриптор родительского окна и экземпляра приложение
		HWND_DESKTOP, NULL, hThisInst, NULL);
	//====================================
	//создание текстового поля
	label = CreateWindow(
		"static",//имя класса
		"",//начальный текст
		//флаги, отвечающие за стиль текстового поля:
		//дочернее окно, первоначально видимо
		WS_CHILD | WS_VISIBLE,
		5, 5,//горизонтальное и вертикальное положение текстового поля в окне
		350, 100,//ширина и высота текстового поля
		//дескриптор родительского окна и экземпляра приложение
		hwnd, NULL, hThisInst, NULL);
	//====================================
	//создание потока, в котором вызывается функция ThreadFunc
	hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &IDThread);
	//закрываем поток, удаляем дескриптор
	CloseHandle(hThread);
	//====================================
	//создание кнопки "Обновить"
	CreateWindow(
		"button",//имя класса
		"Обновить",//начальный текст
		//флаги, отвечающие за стиль кнопки:
		//дочернее окно, первоначально видимо, обработка нажатия в WM_COMMAND
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		145, 110,//горизонтальное и вертикальное положение кнопки в окне
		85, 30,//ширина и высота кнопки
		//дескриптор родительского окна и экземпляра приложение
		hwnd, NULL, hThisInst, NULL);
	//====================================
	ShowWindow(hwnd, nWinMode);//показ окна на экране
	//====================================
	//обработка поступающих сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
