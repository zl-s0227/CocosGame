// CocosGame.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "AppDelegate.h"
#include <tchar.h>

USING_NS_CC;

int WINAPI _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	AppDelegate app;
	return Application::getInstance()->run();
}