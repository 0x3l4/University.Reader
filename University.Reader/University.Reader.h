#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100
#define WINDOW_X 600
#define WINDOW_Y 300
#define BASE_MARGIN 10

#define IDC_EDIT_FILE 201
#define ID_TIMER  202
#define FILE_SIZE 2048

// ���������� ����������:
HINSTANCE hInst;                                // ������� ���������
WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����
HWND hEditFile = NULL;
HANDLE hMapping = INVALID_HANDLE_VALUE;
HANDLE hChange = NULL;
HANDLE hFileUpdateEvent = NULL;
LPVOID mapLP = NULL;
const char* MAPPING_NAME = "Local\\WRFileMapping";
const char* EVENT_NAME = "FileUpdateEvent";

// ��������� ���������� �������, ���������� � ���� ������ ����:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void				OnCreate(HWND);