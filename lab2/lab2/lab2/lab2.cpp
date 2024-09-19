
#include <iostream>
#include <windows.h>

using namespace std;

volatile int* a;
volatile int maxx, minn, aver;


DWORD WINAPI min_max(LPVOID num) {
    int n = (int)num;
    maxx = a[0];
    minn = a[0];
    for (int i = 0; i < n; i++) {
        if (a[i] > maxx) {
            maxx = a[i];
            Sleep(7);
        }
        if (a[i] < minn) {
            minn = a[i];
            Sleep(7);
        }
    }

    cout << "Max element : " << maxx << "\nMin element: " << minn << endl;

    return 0;
}

DWORD WINAPI average(LPVOID num) {
    int n = (int)num;
    int summ=0;
    for (int i = 0; i < n; i++) {
        summ += a[i];
        Sleep(12);
    }

    aver = summ / n;
    cout << "Average: " << aver << endl;

    return 0;
}

int main()
{
    int n, x;
    HANDLE hMinMax, hAverage;
    DWORD	IDMinMax, IDAverage;

    cout << "Type a length of array: ";
    cin >> n;
    a = new int[n];


    cout << "Type your array: ";
    for (int i = 0; i < n; i++) {
        cin >> x;
        a[i] = x;
    }

    hMinMax = CreateThread(NULL,  0, min_max, (void*)n, 0, &IDMinMax);
    hAverage = CreateThread(NULL, 0, average, (void*)n, 0, &IDAverage);
    if (hMinMax == NULL || hAverage == NULL) return GetLastError();

    WaitForSingleObject(hMinMax, INFINITE);
    CloseHandle(hMinMax);

    WaitForSingleObject(hAverage, INFINITE);
    CloseHandle(hAverage);

    cout << "Array, that we get after editing: " << endl;
    for (int i = 0; i < n; i++) {
        if (a[i] == maxx || a[i] == minn) a[i] = aver;
        cout << a[i] << " ";
    }
}
