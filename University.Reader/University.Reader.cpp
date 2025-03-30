#pragma comment(lib, "Comctl32.lib")

#include "framework.h"
#include "University.Reader.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UNIVERSITYREADER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNIVERSITYREADER));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNIVERSITYREADER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_UNIVERSITYREADER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

    HWND hWnd = CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_X, WINDOW_Y, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CREATE:
        OnCreate(hWnd);
        break;
    case WM_TIMER: {
        if (wParam == ID_TIMER) {
            if (hFileUpdateEvent && WaitForSingleObject(hFileUpdateEvent, 0) == WAIT_OBJECT_0)
            {
                char buffer[FILE_SIZE];
                memcpy(buffer, mapLP, FILE_SIZE);
                buffer[FILE_SIZE - 1] = '\0'; // гарантируем корректное завершение строки

                // Обновляем содержимое EDIT-контрола, если данные изменились
                char currentText[FILE_SIZE];
                SetWindowTextA(hEditFile, "");
                GetWindowTextA(hEditFile, currentText, FILE_SIZE);
                if (strcmp(buffer, currentText) != 0)
                    SetWindowTextA(hEditFile, buffer);
            }
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pmmi = (MINMAXINFO*)lParam;
        pmmi->ptMinTrackSize.x = WINDOW_X;
        pmmi->ptMinTrackSize.y = WINDOW_Y;
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
    {
        if (mapLP) UnmapViewOfFile(mapLP);
        if (hMapping) CloseHandle(hMapping);
        if (hFileUpdateEvent) CloseHandle(hFileUpdateEvent);
        KillTimer(hWnd, ID_TIMER);
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void OnCreate(HWND hWnd) {
    InitCommonControls();

    hEditFile = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        BASE_MARGIN, BASE_MARGIN, WINDOW_X - BASE_MARGIN * 2, WINDOW_Y - BASE_MARGIN * 2, hWnd, (HMENU)IDC_EDIT_FILE, hInst, NULL);
    if (hEditFile == NULL) {
        MessageBox(hWnd, L"Не удалось создать элемент управления.", L"Ошибка", MB_ICONERROR);
        return;
    }

    hMapping = OpenFileMappingA(FILE_MAP_READ, FALSE, MAPPING_NAME);
    if (!hMapping) {
        MessageBoxA(NULL, "Ошибка открытия отображения файла", "Ошибка", MB_OK);
        return;
    }

    mapLP = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, FILE_SIZE);
    if (!mapLP) {
        MessageBoxA(NULL, "Ошибка маппинга файла", "Ошибка", MB_OK);
        CloseHandle(hMapping);
        return;
    }

    hFileUpdateEvent = OpenEventA(SYNCHRONIZE, FALSE, EVENT_NAME);
    if (!hFileUpdateEvent) {
        MessageBoxA(NULL, "Ошибка открытия именого события", "Ошибка", MB_OK);
    }

    SetTimer(hWnd, ID_TIMER, 100, NULL);
}