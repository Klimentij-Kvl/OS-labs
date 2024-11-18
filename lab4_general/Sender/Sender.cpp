// Sender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

int main(int argc, char *argv[])
{
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"SyncMutex");
    HANDLE hFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"FullSemaphore");
    HANDLE hEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"EmptySemaphore");
    if (hMutex == NULL) {
        cout << "Open mutex error";
        return GetLastError();
    }
    if (hFull == 0) {
        cout << "Open semaphore named full error";
        return GetLastError();
    }
    if (hEmpty == 0) {
        cout << "Open semaphore named empty error";
        return GetLastError();
    }
    

    WaitForSingleObject(hMutex, INFINITE);
    ReleaseMutex(hMutex);
    string ans;
    while (true) {
        cout << "Type your operation:" << endl;
        cin >> ans;
        
        if (ans == "exit") break;
        else {
            WaitForSingleObject(hEmpty, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);
            ofstream out(argv[1], ios::app, ios::binary);
            out.write(ans.data(), 20);
            out.close();
            ReleaseMutex(hMutex);
            ReleaseSemaphore(hFull, 1, NULL);
        }
    }
    return 0;
}