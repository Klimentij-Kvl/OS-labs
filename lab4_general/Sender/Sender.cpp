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
    
    string ans;
    while (true) {
        cout << "Type your operation:" << endl;
        cin >> ans;
        
        if (ans == "exit") break;
        else {
            
            WaitForSingleObject(hMutex, INFINITE);
            ofstream out(argv[1], ios::app, ios::binary);
            out.write(ans.data(), 20);
            out.close();
            ReleaseMutex(hMutex);
        }
    }
    return 0;
}