#include <windows.h>
#include <ctime>
#include <cwchar>

#include "WcharSaying.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Draw(HDC hdc);
wchar_t* getTime(wchar_t*);
wchar_t* getGaokao(wchar_t*);
const wchar_t* getSaying(int);
size_t getGaokao();

namespace cllData
{
    LPCWSTR szAppName = L"ScreenSaver";
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    size_t gaokao = 0;
    // 
    int W = GetSystemMetrics(SM_CXSCREEN);
    int H = GetSystemMetrics(SM_CYSCREEN);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    using namespace cllData;
    
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    // ע�ᴰ����
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, L"Failed to register window class!", szAppName, MB_ICONERROR);
        return 0;
    }

    // ��������
    hwnd = CreateWindowEx(WS_EX_TOPMOST,
                        szAppName, L"Screen Saver",
                        WS_POPUP | WS_VISIBLE ,
                        0, 0,
                        W , H,
                        NULL, NULL,
                        hInstance, NULL);

    if (!hwnd)
    {
        MessageBox(NULL, L"Failed to create window!", szAppName, MB_ICONERROR);
        return 0;
    }
    // �ڴ��ڴ�����ĳ�ʼ�����������ö�ʱ��
    SetTimer(hwnd, /*��ʱ��ID*/1,1000,NULL); // ��ʱ�����̺���


    // ��Ϣѭ��
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient, cyClient;
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_TIMER:
        // ÿ���Ӵ���һ�Σ�ִ���ػ��߼�
            if (wParam == 1)
                InvalidateRect(hwnd, NULL, TRUE); // ǿ���ػ���������
        return 0;

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        Draw(hdc);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_KEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    //case WM_MOUSEMOVE:
        PostQuitMessage(0);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void Draw(HDC hdc)
{
    RECT rect;
    GetClientRect(cllData::hwnd, &rect); // ��ȡ���ڵĿͻ���
    HBRUSH hBrush = CreateSolidBrush(RGB(10, 10, 10)); // ������ɫ��ˢ
    FillRect(hdc, &rect, hBrush); // ����ɫ����
    DeleteObject(hBrush); // ɾ����ˢ

    SetTextColor(hdc, RGB(255, 255, 255)); // �����ı���ɫΪ��ɫ
    SetBkMode(hdc, TRANSPARENT); // �����ı�����͸��

    rect.top += cllData::H/10;
    // ��������
    HFONT hFont = CreateFont(
        cllData::H/5,                   // ����߶�
        cllData::W/30,                    // ������
        0,                    // ������б�Ƕ�
        0,                    // ������б�Ƕ�
        FW_NORMAL,            // �����ϸ
        FALSE,                // �Ƿ�б��
        FALSE,                // �Ƿ����»���
        FALSE,                // �Ƿ���ɾ����
        DEFAULT_CHARSET,      // �ַ���
        OUT_DEFAULT_PRECIS,   // �������
        CLIP_DEFAULT_PRECIS,  // �ü�����
        DEFAULT_QUALITY,      // ��������
        DEFAULT_PITCH | FF_DONTCARE, // ������ͼ���
        L"Arial"              // ��������
    );
    // ѡ�����嵽�豸������
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    
    // ���Ƶ�ǰʱ��
    wchar_t a[20];
    DrawTextW(hdc, getTime(a), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // ���Ƶ���ʱ
    wchar_t b[20];
    rect.top += cllData::H/3;// �ƶ�����ʱ�ı���λ��
    DrawTextW(hdc, getGaokao(b), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // �ָ�ԭ��������
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    // �߿���־��¼
    rect.top += cllData::H/5+20; // �ƶ��������Ƶ�λ��
    DrawTextW(hdc, getSaying(getGaokao()), -1, &rect, DT_WORDBREAK | DT_CENTER | DT_TOP | DT_SINGLELINE);

}

wchar_t *getTime(wchar_t* timeString)
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    wcsftime(timeString, 20, L"������: %H:%M:%S", &localTime);
    // MessageBox(cllData::hwnd, timeString, L"Information", MB_OK | MB_ICONINFORMATION);
    return timeString;
}

size_t getGaokao()
{
    if(cllData::gaokao != 0)
        return cllData::gaokao;
        
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    
    tm gaokao = {0};
    gaokao.tm_year = localTime.tm_year;
    gaokao.tm_mon = 5; // 6�µ��·�����Ϊ5
    gaokao.tm_mday = 7;

    if (mktime(&gaokao) < now)
        gaokao.tm_year += 1;

    double secondsUntilGaokao = difftime(mktime(&gaokao), now);
    return  static_cast<int>(secondsUntilGaokao / (60 * 60 * 24));
}

wchar_t *getGaokao(wchar_t* countdownString)
{
    
    swprintf(countdownString, 20, L"����߿����� %d ��", getGaokao());
    // MessageBox(cllData::hwnd, countdownString, L"Information", MB_OK | MB_ICONINFORMATION);
    return countdownString;
}

const wchar_t* getSaying(int day)
{
    const size_t i = day % cllData::avrn;
    return cllData::avrg[i];
}