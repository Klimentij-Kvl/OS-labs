// Sender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;


int main(int argc, char *argv[])
{
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"SyncMutex");
    if (hMutex == NULL) {
        cout << "Open mutex error";
        return GetLastError();
    }

    fstream(argv[1], ios::binary);

    WaitForSingleObject(hMutex, INFINITE);
    cout << "lol kek" << endl;
    ReleaseMutex(hMutex);
    return 0;
}