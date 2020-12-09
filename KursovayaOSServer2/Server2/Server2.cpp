// KursovayaOSServer2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <sstream>
#include <thread>
HANDLE g_hFileMap;
HANDLE g_hEvent;
HANDLE g_hExitEvent;
bool toExit;

void SendData()
{
	LPVOID fileMap = NULL;

	static DWORD procID = GetProcessId(GetCurrentProcess());
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);;
	if (handle == NULL)
	{
		handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	}
	std::wostringstream wss;
	fileMap = MapViewOfFile(g_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 512);
	std::cout << "Writing data into the file map...\n" << std::endl;
	wss << "Current process ID of the server: " << procID << "\nHandle pseudo descriptor: 0x" << std::hex << handle << std::endl;
	std::wcout << wss.str().c_str() << std::endl;
	CopyMemory(fileMap, wss.str().c_str(), wss.str().length() * sizeof(TCHAR));
	SetEvent(g_hEvent);
	CloseHandle(handle);
	UnmapViewOfFile(fileMap);
}
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
	g_hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 512, L"Local\\FileMap2");
	g_hEvent = CreateEvent(NULL, FALSE, FALSE, L"Event2");
	g_hExitEvent = CreateEvent(NULL, FALSE, FALSE, L"ExitEvent2");
	std::cout << "Welcome to the File Mapping Server #2!\n" << std::endl;
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
		MUTEX_ALL_ACCESS, 0, L"Server2");
	if (!hMutex)
		hMutex =
		CreateMutex(0, 0, L"Server2");
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
