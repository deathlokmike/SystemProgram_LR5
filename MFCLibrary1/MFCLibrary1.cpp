// MFCLibrary1.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "MFCLibrary1.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <afxsock.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CMFCLibrary1App
struct Header
{
	int h_size;
	int h_thread;
};

CSocket* Socket;
CSocket Server;


extern "C"
{
	//int _afxForceUSRDLL;
	_declspec(dllexport) void _stdcall Init()
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
	}
	_declspec(dllexport) void _stdcall workWithClients()
	{
		AfxSocketInit();
		Server.Create(12345);
	}
	_declspec(dllexport) int _stdcall GetInt()
	{
		int i;
		Socket->Receive(&i, sizeof(int));
		return i;
	}
	_declspec(dllexport) void _stdcall SendInt(int n)
	{
		Socket->Send(&n, sizeof(int));
	}
	_declspec(dllexport) char* _stdcall GetString(int& threadNumber)
	{
		Header header;
		char buff[sizeof(Header)];

		Socket->Receive(buff, sizeof(Header));
		memcpy(&header, buff, sizeof(Header));

		char* str = new char[header.h_size];
		threadNumber = header.h_thread;

		Socket->Receive(str, header.h_size);
		return str;
	}

	_declspec(dllexport) void _stdcall SendString(char* str, int threadNumber)
	{
		Header header = { strlen(str) + 1, threadNumber };

		Socket->Send(&header, sizeof(Header));
		Socket->Send(str, header.h_size);
	}
	_declspec(dllexport) void _stdcall disconnect()
	{
		Socket->Close();
		delete Socket;
	}
	_declspec(dllexport) void _stdcall connectToServer()
	{
		Socket = new CSocket;
		Socket->Create();

		Socket->Connect("127.0.0.1", 12345);
	}
	_declspec(dllexport) inline void _stdcall Connect()
	{
		Socket = new CSocket;
		Server.Listen();
		Server.Accept(*Socket);
	}
	_declspec(dllexport) inline void _stdcall serverDisconnect()
	{
		Socket->Close();
		delete Socket;
	}

	_declspec(dllexport) void _stdcall LaunchClient()
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		CreateProcess(NULL, (LPSTR)"Form1.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
BEGIN_MESSAGE_MAP(CMFCLibrary1App, CWinApp)
END_MESSAGE_MAP()


// Создание CMFCLibrary1App

CMFCLibrary1App::CMFCLibrary1App()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CMFCLibrary1App

CMFCLibrary1App theApp;
BOOL APIENTRY MFCLibrary1(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//Cleanup();
		break;
	}
	return TRUE;
}

// Инициализация CMFCLibrary1App

BOOL CMFCLibrary1App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
