// Lab1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "Lab1.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Единственный объект приложения
CWinApp theApp;
HANDLE hEventAllSend = CreateEvent(NULL, TRUE, FALSE, NULL);
vector<HANDLE> vCloseEvent;
vector<HANDLE> vMessageEvent;
HANDLE hMutex = CreateMutex(NULL, FALSE, "My_mutex");
extern "C"
{
	__declspec(dllimport) void __stdcall LaunchClient();
	__declspec(dllimport) void __stdcall workWithClients();
	__declspec(dllimport) inline int __stdcall GetInt();
	__declspec(dllimport) inline char* __stdcall GetString(int& threadNumber);
	__declspec(dllimport) inline void __stdcall SendInt(int n);
	__declspec(dllimport) inline void __stdcall Connect();
	__declspec(dllimport) inline void __stdcall serverDisconnect();
}

void ToFile(int num, string& message)
{
	string fname = to_string(num + 1) + ".txt";
	ofstream fout(fname);
	fout << message;
	fout.close();
}
string message = "";
UINT MyThread(LPVOID LPParametr)
{
	int num = (int)LPParametr;
	WaitForSingleObject(hMutex, INFINITE);
	cout << "Поток № " << num + 1 << " создан." << endl;
	ReleaseMutex(hMutex);
	HANDLE hEvents[] = { vCloseEvent[num], vMessageEvent[num], hEventAllSend };
	bool flag = false;
	while (1)
	{
		switch (WaitForMultipleObjects(3, hEvents, FALSE, INFINITE) - WAIT_OBJECT_0)
		{
		case 0:
		{
			WaitForSingleObject(hMutex, INFINITE);
			cout << "Поток № " << num + 1 << " завершен." << endl;
			ReleaseMutex(hMutex);
			flag = true;
			break;
		}
		case 1:
		case 2:
		{
			WaitForSingleObject(hMutex, INFINITE);
			ToFile(num, message);
			ReleaseMutex(hMutex);
			break;
		}
		}
		if (flag)
			break;
	}
	return 0;
}

void start()
{
	int k = 0;
	while (true)
	{
		Connect();
		int evNum = GetInt();
		switch (evNum) {
		case 0:
		{
			const int threadsNumber = GetInt();
			for (int i = 0; i < threadsNumber; i++)
			{
				thread t(MyThread, (LPVOID)(k++));
				t.detach();
				vCloseEvent.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));
				vMessageEvent.push_back(CreateEvent(NULL, FALSE, FALSE, NULL));

			}
			break;
		}
		case 1:
		{

			SetEvent(vCloseEvent[--k]);
			CloseHandle(vCloseEvent[k]);
			vCloseEvent.pop_back();
			break;
		}
		case 2:
		{
			int threadNumber;
			char* tmp = GetString(threadNumber);
			message = string(tmp);
			delete[] tmp;
			switch (threadNumber)
			{
			case -1:
			{
				SetEvent(hEventAllSend);
				break;
			}
			case -2:
			{
				cout << "\t\tСообщение от главного потока: " << message << "." << endl;
				break;
			}
			default:
			{
				SetEvent(vMessageEvent[threadNumber]);
				break;
			}
			}
		}
		}
		SendInt(vCloseEvent.size());
		serverDisconnect();
	}
}
int main()
{
    setlocale(LC_ALL, "");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int nRetCode = 0;
    HMODULE hModule = ::GetModuleHandle(nullptr);
    cout << "Консольное приложение запущено. " << endl;
    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
			//LaunchClient();
			workWithClients();
			start();
        }
    }
    else
    {
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}