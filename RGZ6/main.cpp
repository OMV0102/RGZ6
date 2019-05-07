#include <windows.h>
#include <stdio.h>
using namespace std;
#define LIB "library.dll" //имя подключаемой скомпилированной библиблиотеки

LPCSTR szClassName = "Control Source";//имя класса
LPCSTR szTitle = "MAX высота окна";//заголовок окна

HWND hwnd;//дескриптор окна
HWND label_height, label_sse;

char info[256];

DWORD WINAPI ThreadFunc(void*)
{

    HINSTANCE hinstLib = LoadLibrary(TEXT(LIB));//загружаем динамическую библиотеку
    if(hinstLib != NULL)
    {
        typedef int (*ImportFunction)();
		//в переменную win_height запоминаем результат работы функции win_height
		ImportFunction win_height = (ImportFunction)GetProcAddress(hinstLib, "win_height");
		
		//если функция win_height вернула какое-то значение
        if(win_height != NULL)
        {
            lbl << "Basic task:" << endl <<
                "Maximum height of the window = " << get_max_window_height();
            SetWindowText(label_height, (lbl.str()).c_str());
        }
		//если функция win_height вернула значение NULL
        else
        {
            msg << "int get_max_window_height() not found in " << LIB;
            MessageBox(hwnd, (msg.str()).c_str(), "Error", MB_OK | MB_ICONERROR);
        }
        
		//в переменную support_sse запоминаем результат работы функции support_sse
		ImportFunction support_sse  = (ImportFunction)GetProcAddress(hinstLib, "support_sse");

        if(support_sse != NULL)
        {
            int sse = has_sse();
            stringstream lbl;
            lbl << "Advanced task:" << endl << "Streaming SIMD Extensions status: ";
            if(sse > 0)
                lbl << "SSE supported";
            else if(sse == 0)
                lbl << "SSE not supported";
            else
                lbl << "can`t determine - CPUID not support";
            SetWindowText(label_sse, (lbl.str()).c_str());
        }
        else
        {
            stringstream msg;
            msg << "int has_sse() not found in " << LIB;
            MessageBox(hwnd, (msg.str()).c_str(), "Error", MB_OK | MB_ICONERROR);
        }
        FreeLibrary(hinstLib);
    }
    else
    {
        stringstream msg;
        msg << LIB << " not found";
        MessageBox(hwnd, (msg.str()).c_str(), "Error", MB_OK | MB_ICONERROR);
    }
	return 0;
}

/**
 * @brief Function for handling messages
 * @param hWnd handle to window procedure which received message.
 * @param Msg message
 * @param wParam additional message information
 * @param lParam additional message information
 * @return result of message processing and depends on message
 */
LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CTLCOLORSTATIC: /** Change color */
    {
        DWORD CtrlID = GetDlgCtrlID((HWND)lParam);
        if (CtrlID == 1001 || CtrlID == 1002)
        {
            SetTextColor((HDC)wParam, RGB(200, 200, 200));
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            return (INT_PTR)GetStockObject(BLACK_BRUSH);
        }
    }
    break;
    case WM_COMMAND: /** Button clicked */
    {
        if (LOWORD(wParam) == 1003)
        {
            HANDLE hThread;
            DWORD IDThread;
            /** Launch in new thread */
            hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread);
            CloseHandle(hThread);
        }
        if (LOWORD(wParam) == 1004)
            PostQuitMessage(0);
    }
    break;
    case WM_DESTROY: /** Window closed */
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

/**
 * @brief Main function
 * @param hInstance handle to current instance of application
 * @param hPrevInstance handle to previous instance of application (always NULL)
 * @param lpCmdLine command line for application, excluding the program name
 * @param nCmdShow controls how window is to be shown
 * @return 0
 */
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
    /** Some fonts */
    HFONT font_std = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT font_mono = (HFONT)GetStockObject(OEM_FIXED_FONT);
    /** Some definition for main_class */
    WNDCLASS wnd;
    memset(&wnd, 0, sizeof(WNDCLASS));
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = WindowFunc;
    wnd.hInstance = hThisInst;
    wnd.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wnd.lpszClassName = szClassName;
    wnd.hIcon = LoadIcon(hThisInst, MAKEINTRESOURCE(100));
    RegisterClass(&wnd);
    /** Some main window */
    int window_width = 300;
    int window_height = 200;
    int button_width = 75;
    int button_height = 25;
    int border = 5;
    HDC hDCScreen = GetDC(NULL);
    hwnd = CreateWindow(szClassName, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (GetDeviceCaps(hDCScreen, HORZRES) - window_width) / 2, (GetDeviceCaps(hDCScreen, VERTRES) - window_height) / 2,
        window_width, window_height, NULL, NULL, hInstance, NULL);
    RECT rt;
    GetClientRect(hwnd, &rt);
    window_width = rt.right;
    window_height = rt.bottom;
    /** Some static labels for basic and advanced tasks */
    label_height = CreateWindow("static", "Click \"Run\"", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, border, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1001, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1001, WM_SETFONT, (WPARAM)font_mono, TRUE);
    label_sse = CreateWindow("static", "Click \"Run\"", WS_CHILD | WS_VISIBLE | WS_BORDER,
        border, (window_height - button_height) / 2, window_width - 2 * border, (window_height - button_height - 4 * border) / 2,
        hwnd, (HMENU)1002, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1002, WM_SETFONT, (WPARAM)font_mono, TRUE);
    /** Running thread before show window */
    HANDLE hThread;
    DWORD IDThread;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread, NULL, 0, &IDThread);
    CloseHandle(hThread);
    /** Some buttons: Run and Exit */
    CreateWindow("button", "Run", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        window_width - 2 * (border + button_width), window_height - border - button_height, button_width, button_height,
        hwnd, (HMENU)1003, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1003, WM_SETFONT, (WPARAM)font_std, TRUE);
    CreateWindow("button", "Exit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        window_width - border - button_width, window_height - border - button_height, button_width, button_height,
        hwnd, (HMENU)1004, hInstance, NULL);
    SendDlgItemMessage(hwnd, 1004, WM_SETFONT, (WPARAM)font_std, TRUE);
    /** Starting ... */
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
