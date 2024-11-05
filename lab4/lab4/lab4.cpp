#include <iostream>
#include <windows.h>
#include <list>

using namespace std;

list<int> arr;

CRITICAL_SECTION cs;


HANDLE *hProducers, *hConsumers;
HANDLE hEmpty, hCounter;
bool* workingCons;

volatile int consumersNum, producersNum;

DWORD WINAPI producer(LPVOID Nu) {
    int num = (int)Nu;
    srand(num);

    int limit = rand() % 10 + 1;
    for (int i = 0; i < limit; i++) {
        Sleep((rand() % 3 + 2) * 1000);
        WaitForSingleObject(hEmpty, INFINITE);
        EnterCriticalSection(&cs);
        arr.push_back(num);
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            cout << *it << " ";
        }
        cout << "|added a " << num << endl;
        
        LeaveCriticalSection(&cs);
        ReleaseSemaphore(hCounter, 2, NULL);
    }

    EnterCriticalSection(&cs);
    cout << "    !producer " << num << " ended his work" << endl;
    workingCons[num] = true;

    LeaveCriticalSection(&cs);

    return 0;
}

DWORD WINAPI consumer(LPVOID Nu) {
    int num = ((int)Nu + 1)  % producersNum;
    srand((int)Nu);
    bool b;

    while (true) {
        Sleep((rand() % 3 + 3) * 1000);
        WaitForSingleObject(hCounter, INFINITE);
        EnterCriticalSection(&cs);
        b = true;
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            if (*it == num) {
                arr.erase(it);
                for (auto it = arr.begin(); it != arr.end(); ++it) {
                    cout << *it << " ";
                }
                cout << "|deleted a " << num << " by " << (int)Nu << endl;
                
                b = false;
                break;
            }
        }
        LeaveCriticalSection(&cs);
        
        if (b == false) {
            ReleaseSemaphore(hCounter, 1, NULL);
            ReleaseSemaphore(hEmpty, 1, NULL);
        }
        if (b == true && workingCons[num] == true) {
            break;
        }
    }

    EnterCriticalSection(&cs);
    cout << "    !consumer " << num << " ended his work" << endl;
    LeaveCriticalSection(&cs);

    return 0;
}

int main()
{
    DWORD *IDConsumers, *IDProducers;
    InitializeCriticalSection(&cs);

    int a, b;
    cout << "Type number of producers and consumers: ";
    cin >> a >> b;
    producersNum = a; consumersNum = b;

    hConsumers = new HANDLE[consumersNum];
    IDConsumers = new DWORD[consumersNum];
    hProducers = new HANDLE[producersNum];
    IDProducers = new DWORD[producersNum];
    workingCons = new bool[producersNum];

    hEmpty = CreateSemaphore(NULL, 15, 15, NULL);
    hCounter = CreateSemaphore(NULL, 0, 15, NULL);

    for (int i = 0; i < producersNum; i++) {
        hProducers[i] = CreateThread(NULL, 0, producer, (LPVOID)(i), 0, &IDProducers[i]);
        if (hProducers[i] == NULL) return 1;

        workingCons[i] = false;
    }

    for (int i = 0; i < consumersNum; i++) {
        hConsumers[i] = CreateThread(NULL, 0, consumer, (LPVOID)(i), 0, &IDConsumers[i]);
        if (hConsumers[i] == NULL) return 1;
    }

    WaitForMultipleObjects(consumersNum, hConsumers, true, INFINITE);

    for (int i = 0; i < producersNum; i++) {
        CloseHandle(hProducers[i]);
    }
    
    for (int i = 0; i < consumersNum; i++) {
        CloseHandle(hConsumers[i]);
    }
    
    CloseHandle(hEmpty);
    CloseHandle(hCounter);
    DeleteCriticalSection(&cs);
}

