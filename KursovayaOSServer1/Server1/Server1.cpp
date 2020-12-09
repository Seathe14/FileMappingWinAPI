// KursovayaOSServer1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <sstream>
#include <tchar.h>
#include <thread>
HANDLE g_hFileMap;
HANDLE g_hEvent;
HANDLE g_hExitEvent;
bool toExit;
void SendData() 
{ 

	LPVOID fileMap = NULL;
	DWORD sysColors[3];
	DWORD dwThreadID = GetCurrentThreadId();
	short hCurKeyboard = (short)GetKeyboardLayout(dwThreadID);
	sysColors[0] = GetSysColor(COLOR_WINDOW);
	sysColors[1] = GetSysColor(COLOR_ACTIVECAPTION);
	sysColors[2] = GetSysColor(COLOR_WINDOWFRAME);
    fileMap = MapViewOfFile(g_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 512);
	std::cout << "Writing data into the file map...\n" << std::endl;
    std::wostringstream wss;
    wss << "Current window color: " << std::hex << "0x" << GetRValue(sysColors[0]) << ", 0x" << GetGValue(sysColors[0]) << ", 0x" << GetBValue(sysColors[0]) << std::endl;
	wss << "Current active caption color: " << "0x" << GetRValue(sysColors[1]) << ", 0x" << GetGValue(sysColors[1]) << " 0x" << GetBValue(sysColors[1]) << std::endl;
	wss << "Current window frame color: " << "0x" << GetRValue(sysColors[2]) << ", 0x" << GetGValue(sysColors[2]) << ", 0x" << GetBValue(sysColors[2]) << std::endl;
    wss << "Current keyboard layout: " << "0x" << hCurKeyboard << std::endl;
	std::wcout << wss.str() << std::endl;
    CopyMemory(fileMap, wss.str().c_str(), wss.str().length() * sizeof(TCHAR));
    SetEvent(g_hEvent);
    UnmapViewOfFile(fileMap);
};
void mainLoop()
{

	while (true)
	{
		WaitForSingleObject(g_hEvent, INFINITE);
		if (toExit)
			break;
		SendData();
		Sleep(1);
	}
}
void checkExit()
{
	WaitForSingleObject(g_hExitEvent, INFINITE);
	std::cout << "Server is closing..." << std::endl;
	toExit = true;
	SetEvent(g_hEvent);
}
void init()
{
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, L"Event");
	g_hExitEvent = CreateEvent(NULL, FALSE, FALSE, L"ExitEvent");
	g_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 512, L"Local\\FileMap");
	std::cout << "Welcome to the File Mapping Server #1\n\n";

}
void closeHandles()
{
	CloseHandle(g_hEvent);
	CloseHandle(g_hExitEvent);
	CloseHandle(g_hFileMap);
}
int main()
{
	HANDLE hMutex = OpenMutex(
		MUTEX_ALL_ACCESS, 0, L"Server1");
	if (!hMutex)
		hMutex =
		CreateMutex(0, 0, L"Server1");
	else
		return 0;
	init();
	std::thread t1(mainLoop);
	std::thread t2(checkExit);
	t1.join();
	t2.join();
	closeHandles();
	system("pause");
}
