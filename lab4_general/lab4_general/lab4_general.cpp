// lab4_general.cpp : This file contains the 'main' function. Program execution begins and ends t#include <iostream>
#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int main()
{
    HANDLE hMutex;
    HANDLE hSemaphore;
    STARTUPINFO* si;
    PROCESS_INFORMATION* pi;
    wchar_t fileName[50];

    wstring fileN;
    int recNum, receiverNum;
    cout << "Print file name and number of records: " << endl;
    fileN = L"rofl.bin"; recNum = 5; //wcin >> fileN >> recNum; 
    fstream file(fileN, ios::binary);
    wstring s = L"Sender.exe " + fileN;
    lstrcpyW(fileName, s.data());

    hMutex = CreateMutex(NULL, FALSE, L"SyncMutex");
    hSemaphore = CreateSemaphore(NULL, 0, recNum, L"SyncSemaphore");
    if (hMutex == NULL) {
        cout << "MUTEX ERROR" << endl;
        return GetLastError();
    }
    WaitForSingleObject(hMutex, INFINITE);
    cout << "Print number of receivers: " << endl;
    receiverNum = 10; //cin >> receiverNum;
    si = new STARTUPINFO[receiverNum];
    pi = new PROCESS_INFORMATION[receiverNum];
    for (int i = 0; i < receiverNum; i++) {
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, fileName, NULL, NULL, FALSE, NULL, NULL, NULL, &si[i], &pi[i])) {
            cout << "Process " << i << " error";
            return GetLastError();
        }
    }
    Sleep(100);
    WaitForMultipleObjects(receiverNum, &pi->hProcess, TRUE, INFINITE);
    ReleaseMutex(hMutex);
    WaitForMultipleObjects(receiverNum, &pi->hProcess, TRUE, INFINITE);

    for (int i = 0; i < receiverNum; i++) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    CloseHandle(hMutex);
    CloseHandle(hSemaphore);
}