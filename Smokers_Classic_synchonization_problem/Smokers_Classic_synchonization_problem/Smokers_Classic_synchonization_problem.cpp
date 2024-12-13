#include <iostream>
#include <windows.h>
#include <ctime>

#define SERVANT_WAIT 800
#define SMOKING_TIME 2000
#define TAKING_FROM_TABLE 200

using namespace std;

HANDLE hTobaccoPaper, hTobaccoMatches, hPaperMatches, hSmoking;
int* smokersAlive;
volatile int smokersAliveCount = 3;

DWORD WINAPI servant(LPVOID) {
    srand(time(0));
    int r;
    while (smokersAliveCount > 0) {
        r = rand() % 3;
        if (smokersAlive[r] < 10) {
            switch (r) {
            case 0:
                ReleaseSemaphore(hTobaccoPaper, 1, NULL);
                cout << "Servant put Tobacco and Paper on the table.\n";
                break;
            case 1:
                ReleaseSemaphore(hTobaccoMatches, 1, NULL);
                cout << "Servant put Matches and Tobacco on the table.\n";
                break;
            case 2:
                ReleaseSemaphore(hPaperMatches, 1, NULL);
                cout << "Servant put Paper and Matches on the table.\n";
                break;
            }
            WaitForSingleObject(hSmoking, INFINITE);
            Sleep(SERVANT_WAIT);
        }
    }
    cout << "!!!Servent dead.\n";
    return 0;
}

DWORD WINAPI smoker1(LPVOID) {
    for (int i = 0; i < 10; i++) {
        WaitForSingleObject(hTobaccoPaper, INFINITE);
        Sleep(TAKING_FROM_TABLE);
        smokersAlive[0]++;
        ReleaseSemaphore(hSmoking, 1, NULL);
        cout << "Smoker with Matches is smoking...\n";
        Sleep(SMOKING_TIME);
        cout << "Smoker with Matches ended to smoke.\n";
    }
    smokersAliveCount--;
    cout << "!!!Smoker with Matches dead.\n";
    return 0;
}

DWORD WINAPI smoker2(LPVOID) {
    for (int i = 0; i < 10; i++) {
        WaitForSingleObject(hTobaccoMatches, INFINITE);
        Sleep(TAKING_FROM_TABLE);
        smokersAlive[1]++;
        ReleaseSemaphore(hSmoking, 1, NULL);
        cout << "Smoker with Paper is smoking...\n";
        Sleep(SMOKING_TIME);
        cout << "Smoker with Paper ended to smoke.\n";
    }
    smokersAliveCount--;
    cout << "!!!Smoker with Paper dead.\n";
    return 0;
}

DWORD WINAPI smoker3(LPVOID) {
    for (int i = 0; i < 10; i++) {
        WaitForSingleObject(hPaperMatches, INFINITE);
        Sleep(TAKING_FROM_TABLE);
        smokersAlive[2]++;
        ReleaseSemaphore(hSmoking, 1, NULL);
        cout << "Smoker with Tobacco is smoking...\n";
        Sleep(SMOKING_TIME);
        cout << "Smoker with Tobacco ended to smoke.\n";
    }
    smokersAliveCount--;
    cout << "!!!Smoker with Tobacco dead.\n";
    return 0;
}

int main()
{
    smokersAlive = new int[3];

    for (int i = 0; i < 3; i++) smokersAlive[i] = 0;

    hTobaccoMatches = CreateSemaphore(NULL, 0, 1, NULL);
    hTobaccoPaper = CreateSemaphore(NULL, 0, 1, NULL);
    hPaperMatches = CreateSemaphore(NULL, 0, 1, NULL);
    hSmoking = CreateSemaphore(NULL, 0, 1, NULL);

    HANDLE* hThreads;
    DWORD* dwThreads;
    hThreads = new HANDLE[4];
    dwThreads = new DWORD[4];
    hThreads[0] = CreateThread(NULL, 0, servant, NULL, 0, &dwThreads[0]);
    hThreads[1] = CreateThread(NULL, 0, smoker1, NULL, 0, &dwThreads[1]);
    hThreads[2] = CreateThread(NULL, 0, smoker2, NULL, 0, &dwThreads[2]);
    hThreads[3] = CreateThread(NULL, 0, smoker3, NULL, 0, &dwThreads[3]);

    if (hThreads[0] == NULL || hThreads[1] == NULL ||
        hThreads[2] == NULL || hThreads[3] == NULL) {
        cerr << "Thread creating error" << endl;
        return 1;
    }

    WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 4; i++) {
        CloseHandle(hThreads[i]);
    }
    CloseHandle(hPaperMatches);
    CloseHandle(hTobaccoMatches);
    CloseHandle(hTobaccoPaper);
    CloseHandle(hSmoking);
    return 0;
}
