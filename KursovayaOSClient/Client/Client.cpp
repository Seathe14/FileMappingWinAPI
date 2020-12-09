// KursovayaOSClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <thread>
#include <mutex>
const TCHAR* names[2][3] = 
{
	{
	_T("Local\\FileMap"),
	_T("Event"),
	_T("ExitEvent")
	},
	{
	_T("Local\\FileMap2"),
	_T("Event2"),
	_T("ExitEvent2")
	},
};
HANDLE g_hFileMaps[2];
HANDLE g_hEvents[2];
HANDLE g_hExitEvents[2];

std::mutex mtx;
void GetDataFromServer(HANDLE fileMap, HANDLE hEvent)
{
	SetEvent(hEvent);
	LPVOID lpFileMap = NULL;
	TCHAR* b;
	WaitForSingleObject(hEvent, INFINITE);
	b = (TCHAR*)MapViewOfFile(fileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	std::wcout << b << std::endl;
	UnmapViewOfFile(b);
}
void CheckServer(HANDLE *hFileMap, HANDLE *hEvent, HANDLE *hExitEvent, const TCHAR** Names)
{
	while (*hFileMap == NULL)
		*hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, Names[0]);
	while (*hEvent == NULL)
		*hEvent = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, Names[1]);
	*hExitEvent = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, Names[2]);

	if (*hFileMap != NULL && *hEvent != NULL)
	{
		if (Names[1] == L"Event")
		{
			mtx.lock();
			std::cout << "Connected to the first server\n" << std::endl;
			mtx.unlock();
		}
		else
		{
			mtx.lock();
			std::cout << "Connected to the second server\n" << std::endl;
			mtx.unlock();
		}
	}
}
void instruction()
{
	std::cout << "Type in operation:\n'1' - GetDataFromServer1\n'2' - GetDataFromServer2\n'3' - CloseServer1\n'4' - CloseServer2\n'5' - cls\n'6' - Exit\n" << std::endl;
}
void closeHandles()
{
	for (int i = 0; i < 2; i++)
	{
		CloseHandle(g_hEvents[i]);
		CloseHandle(g_hExitEvents[i]);
		CloseHandle(g_hFileMaps[i]);
	}
}
int main()
{
    char ch;
	std::cout << "Welcome to the File Mapping Client!\nWaiting for connection to the servers...\n" << std::endl;
	std::thread t1(CheckServer, &g_hFileMaps[0], &g_hEvents[0],&g_hExitEvents[0], names[0]);
	std::thread t2(CheckServer, &g_hFileMaps[1], &g_hEvents[1],&g_hExitEvents[1], names[1]);

	t1.join();
	t2.join();
	instruction();
    do 
    {
        std::cin >> ch;
        switch (ch)
        {
        case '1':
			std::cout << "Reading data from the first file map...\n\n";
            GetDataFromServer(g_hFileMaps[0], g_hEvents[0]);
            break;
        case '2':
			std::cout << "Reading data from the second file map...\n\n";
            GetDataFromServer(g_hFileMaps[1], g_hEvents[1]);
            break;
		case '3':
			SetEvent(g_hExitEvents[0]);
			break;
		case '4':
			SetEvent(g_hExitEvents[1]);
			break;
		case '5':
			system("cls");
			break;
        }
        Sleep(1);
    } while (ch != '6');
	SetEvent(g_hExitEvents[0]);
	SetEvent(g_hExitEvents[1]);

	closeHandles();
}
