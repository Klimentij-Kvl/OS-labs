// lab4_general.cpp : This file contains the 'main' function. Program execution begins and ends t#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

int main()
{
    HANDLE hMutex;
    HANDLE hFull,hEmpty;
    STARTUPINFO* si;
    PROCESS_INFORMATION* pi;
    wchar_t fileName[50];

    wstring fileN;
    int recNum, receiverNum;
    cout << "Print file name and number of records: " << endl;
    wcin >> fileN >> recNum; 
    if (recNum < 1) {
        cout << "incorrect number of records";
        return 1;
    }
    wstring s = L"Sender.exe " + fileN;
    lstrcpyW(fileName, s.data());

    ofstream createBinFile(fileN, ios::binary);
    createBinFile.close();
    

    hMutex = CreateMutex(NULL, FALSE, L"SyncMutex");
    hFull = CreateSemaphore(NULL, 0, recNum, L"FullSemaphore");
    hEmpty = CreateSemaphore(NULL, recNum, recNum, L"EmptySemaphore");
    if (hMutex == NULL) {
        cout << "MUTEX ERROR" << endl;
        return GetLastError();
    }
    WaitForSingleObject(hMutex, INFINITE);
    cout << "Print number of receivers: " << endl;
    cin >> receiverNum;
    if (receiverNum < 1) {
        cout << "incorrect number of receivers";
        return 1;
    }
    si = new STARTUPINFO[receiverNum];
    pi = new PROCESS_INFORMATION[receiverNum];
    HANDLE* hProcessors = new HANDLE[receiverNum];
    ZeroMemory(si, sizeof(STARTUPINFO)*receiverNum);
    for (int i = 0; i < receiverNum; i++) {
        
        si[i].cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, fileName, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
            cout << "Process " << i << " error";
            return GetLastError();
        }
        hProcessors[i] = pi->hProcess;
    }
    Sleep(100);
    if (receiverNum == 1) WaitForSingleObject(&hProcessors, INFINITE);
    else WaitForMultipleObjects(receiverNum, hProcessors, TRUE, INFINITE);
    ifstream inBinFile;
    ofstream outBinFile;
    ReleaseMutex(hMutex);
    string userAns;
    char *fileElem = new char[20];
    string deleting;
    while (true) {
        cout << "type your operation(exit, read):" << endl;
        cin >> userAns;
        
        if (userAns == "exit") {
            for (int i = 0; i < receiverNum; i++) {
                TerminateProcess(pi[i].hProcess, 1);
                TerminateProcess(pi[i].hThread, 1);
            }
            break;
        }
        else {
            WaitForSingleObject(hFull, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);
            inBinFile.open(fileN, ios::binary);
            inBinFile.read(fileElem, 20);
            inBinFile >> deleting;
            if (strcmp(fileElem, deleting.data()) == 0)
                deleting = "";
            inBinFile.close();
            
            outBinFile.open(fileN, ios::binary);
            outBinFile.clear();
            outBinFile.write(deleting.data(), deleting.size());
            outBinFile.close();
            cout << fileElem << endl;
            ReleaseMutex(hMutex);
            ReleaseSemaphore(hEmpty, 1, NULL);
        }
    }
    
    WaitForMultipleObjects(receiverNum, &pi->hProcess, TRUE, INFINITE);

    for (int i = 0; i < receiverNum; i++) {
        CloseHandle(pi[i].hThread);
        CloseHandle(pi[i].hProcess);
        
    }
    CloseHandle(hMutex);
    CloseHandle(hFull);
    CloseHandle(hEmpty);
    inBinFile.close();
}