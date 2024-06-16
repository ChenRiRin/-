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

    // 注册窗口类
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, L"Failed to register window class!", szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建窗口
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
    // 在窗口创建后的初始化代码中设置定时器
    SetTimer(hwnd, /*定时器ID*/1,1000,NULL); // 定时器过程函数


    // 消息循环
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
        // 每秒钟触发一次，执行重绘逻辑
            if (wParam == 1)
                InvalidateRect(hwnd, NULL, TRUE); // 强制重绘整个窗口
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
    GetClientRect(cllData::hwnd, &rect); // 获取窗口的客户区
    HBRUSH hBrush = CreateSolidBrush(RGB(10, 10, 10)); // 创建白色画刷
    FillRect(hdc, &rect, hBrush); // 填充白色背景
    DeleteObject(hBrush); // 删除画刷

    SetTextColor(hdc, RGB(255, 255, 255)); // 设置文本颜色为黑色
    SetBkMode(hdc, TRANSPARENT); // 设置文本背景透明

    rect.top += cllData::H/10;
    // 创建字体
    HFONT hFont = CreateFont(
        cllData::H/5,                   // 字体高度
        cllData::W/30,                    // 字体宽度
        0,                    // 字体倾斜角度
        0,                    // 字体倾斜角度
        FW_NORMAL,            // 字体粗细
        FALSE,                // 是否斜体
        FALSE,                // 是否有下划线
        FALSE,                // 是否有删除线
        DEFAULT_CHARSET,      // 字符集
        OUT_DEFAULT_PRECIS,   // 输出精度
        CLIP_DEFAULT_PRECIS,  // 裁剪精度
        DEFAULT_QUALITY,      // 字体质量
        DEFAULT_PITCH | FF_DONTCARE, // 字体间距和家族
        L"Arial"              // 字体名称
    );
    // 选择字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    
    // 绘制当前时间
    wchar_t a[20];
    DrawTextW(hdc, getTime(a), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // 绘制倒计时
    wchar_t b[20];
    rect.top += cllData::H/3;// 移动倒计时文本的位置
    DrawTextW(hdc, getGaokao(b), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

    // 恢复原来的字体
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    // 高考励志语录
    rect.top += cllData::H/5+20; // 移动屏保名称的位置
    DrawTextW(hdc, getSaying(getGaokao()), -1, &rect, DT_WORDBREAK | DT_CENTER | DT_TOP | DT_SINGLELINE);

}

wchar_t *getTime(wchar_t* timeString)
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    wcsftime(timeString, 20, L"现在是: %H:%M:%S", &localTime);
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
    gaokao.tm_mon = 5; // 6月的月份索引为5
    gaokao.tm_mday = 7;

    if (mktime(&gaokao) < now)
        gaokao.tm_year += 1;

    double secondsUntilGaokao = difftime(mktime(&gaokao), now);
    return  static_cast<int>(secondsUntilGaokao / (60 * 60 * 24));
}

wchar_t *getGaokao(wchar_t* countdownString)
{
    
    swprintf(countdownString, 20, L"距离高考还有 %d 天", getGaokao());
    // MessageBox(cllData::hwnd, countdownString, L"Information", MB_OK | MB_ICONINFORMATION);
    return countdownString;
}

const wchar_t* getSaying(int day)
{
    const size_t i = day % cllData::avrn;
    return cllData::avrg[i];
}