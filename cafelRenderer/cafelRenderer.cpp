// myRenderer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "cafelRenderer.h"
#include "stdio.h"
#include <iostream>
#include <commdlg.h>

#include <cafel/utils/xmlReader.h>
#include <factory/mainfactory.hpp>

#include <cafel/object/glSystem.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

bool console = false;

HWND m_hWnd = nullptr;
HWND hwndGLFW = nullptr;
double scale = 1.0; // 屏幕分辨率缩放比

unsigned char *pic_data = nullptr;
int pic_width, pic_height;

std::thread *glRenderThread, *RenderThread;

cafel::GLSystemApi* gl = nullptr;
cafel::SceneBuilderApi* scene = nullptr;
bool inRender = false;

void reDraw()
{
	RECT rec;
	GetClientRect(m_hWnd, &rec);
	UpdateWindow(m_hWnd);
	RedrawWindow(m_hWnd, &rec, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

void draw(double percent, unsigned char *data=nullptr)
{
	pic_data = data;
	reDraw();
}

void resizeCanvas(HWND hWnd)
{
	RECT rec1;
	GetWindowRect(hWnd, &rec1);
	RECT rec2;
	GetClientRect(hWnd, &rec2);
	int newWidth = (pic_width +(rec1.right - rec1.left) - (rec2.right - rec2.left))*scale;
	int newHeight = (pic_height +(rec1.bottom - rec1.top) - (rec2.bottom - rec2.top))*scale;

	MoveWindow(hWnd, rec1.left, rec1.top, newWidth, newHeight, FALSE);
}

void showGL()
{
	//取得glfw窗口句柄并将其嵌入父窗口
	if (hwndGLFW == nullptr)
	{
		gl = cafel::MainFactory::getGLSystem(scene, scale);

		hwndGLFW = gl->getHWND();
		SetWindowLong(hwndGLFW, GWL_STYLE, WS_VISIBLE);
		MoveWindow(hwndGLFW, 0, 0, pic_width*scale, pic_height*scale, FALSE);
		SetParent(hwndGLFW, m_hWnd);

		glRenderThread = gl->getRenderThread();
	}
}

void hideGL()
{
	if (hwndGLFW != nullptr)
	{
		DestroyWindow(hwndGLFW);
		hwndGLFW = nullptr;
	}
}

void render()
{	
	inRender = true;

	scene->clearData();
	scene->render(draw);

	pic_data = scene->output();
	reDraw();

	scene->saveImg();

	inRender = false;
	printf("finish\n");
}

void openFile()
{
	OPENFILENAME opfn;
	WCHAR strFilename[MAX_PATH];//存放文件名  
	//初始化  
	ZeroMemory(&opfn, sizeof(OPENFILENAME));
	opfn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	//设置过滤  
	opfn.lpstrFilter = L"xml文件\0*.xml\0";
	//默认过滤器索引设为1  
	opfn.nFilterIndex = 1;
	//文件名的字段必须先把第一个字符设为 \0  
	opfn.lpstrFile = strFilename;
	opfn.lpstrFile[0] = '\0';
	opfn.nMaxFile = sizeof(strFilename);
	//设置标志位，检查目录或文件是否存在  
	opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	//opfn.lpstrInitialDir = NULL;  
	// 显示对话框让用户选择文件  
	if (GetOpenFileName(&opfn))
	{
		std::wstring ws(strFilename);
		std::string str(ws.begin(), ws.end());

		cafel::SceneNode *root = cafel::XmlReader::readXmlFile(str);
		scene = cafel::MainFactory::getScene(root);

		pic_width = scene->getWidth();
		pic_height = scene->getHeight();

		resizeCanvas(m_hWnd);
		showGL();
	}
}

void saveFile()
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	TCHAR filename[MAX_PATH] = { 0 };
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"JPG文件\0*.JPG\0";
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
	ofn.hwndOwner = m_hWnd;
	if (GetSaveFileName(&ofn))
	{
		std::wstring ws(filename);
		std::string str(ws.begin(), ws.end());
		scene->saveImg(str);
	}	
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CAFELRENDERER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	/*
	HWND hWnd = GetDesktopWindow();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	// 获取监视器逻辑宽度与高度
	MONITORINFOEX miex;
	miex.cbSize = sizeof(miex);
	GetMonitorInfo(hMonitor, &miex);
	int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);

	// 获取监视器物理宽度与高度
	DEVMODE dm;
	dm.dmSize = sizeof(dm);
	dm.dmDriverExtra = 0;
	EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
	int cxPhysical = dm.dmPelsWidth;

	// 缩放比例计算  实际上使用任何一个即可
	scale = ((double)cxPhysical / (double)cxLogical);
	*/

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAFELRENDERER));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	if(RenderThread != nullptr)
		RenderThread->join();
	if (glRenderThread != nullptr)
		glRenderThread->join();

	if (console)
		FreeConsole(); //释放控制台

    return (int) msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAFELRENDERER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CAFELRENDERER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   m_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, 400, 400, nullptr, nullptr, hInstance, nullptr);

   if (!m_hWnd)
   {
      return FALSE;
   }

   if (console)
   {
	   AllocConsole();
	   FILE *fp_w;
	   freopen_s(&fp_w, "CONOUT$", "w", stdout);//开启中文控制台输出支持 
   }

   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);

   return TRUE;
}

void showJpg(HDC& hdc)
{
	try
	{
		if (pic_data != nullptr)
		{
			//基本信息
			BITMAPINFOHEADER info{ 0 };
			info.biSize = sizeof(info);
			info.biPlanes = 1;
			info.biBitCount = 3 * 8;
			info.biWidth = pic_width;
			info.biHeight = pic_height;
			info.biSizeImage = pic_width * pic_height * 3;

			//设置绘制模式位拉伸模式
			SetStretchBltMode(hdc, HALFTONE);

			unsigned char* image_buffer = new unsigned char[info.biSizeImage];
			for (int i = 0; i < pic_height; i++)
			{
				for (int j = 0; j < pic_width; j++)
				{
					image_buffer[((pic_height - i - 1) * pic_width + j) * 3 + 0] = pic_data[(i* pic_width + j) * 3 + 2];
					image_buffer[((pic_height - i - 1) * pic_width + j) * 3 + 1] = pic_data[(i* pic_width + j) * 3 + 1];
					image_buffer[((pic_height - i - 1) * pic_width + j) * 3 + 2] = pic_data[(i* pic_width + j) * 3 + 0];
				}
			}

			//绘制
			StretchDIBits(hdc, 0, 0, pic_width, pic_height, 0, 0, pic_width, pic_height,
				image_buffer, (LPBITMAPINFO)&info, DIB_RGB_COLORS, SRCCOPY);
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_FILE:
				if (!inRender)
				{
					hideGL();
					openFile();
				}
				break;
			case ID_RENDER:
				if (!inRender)
				{
					if (hwndGLFW == nullptr)
					{						
						showGL();
					}
					else
					{
						hideGL();
						RenderThread = new std::thread(render);
					}
				}
				break;
			case ID_SAVE:
				if (!inRender && pic_data != nullptr)
				{
					saveFile();
				}
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			if (RenderThread != nullptr && inRender)
				showJpg(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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