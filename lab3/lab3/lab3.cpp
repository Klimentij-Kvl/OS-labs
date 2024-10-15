#include <windows.h>
#include <iostream>

using namespace std;

CRITICAL_SECTION cs;
HANDLE *hFinishEvent, hStartEvent, hContinueEvent, hRemoveEvent;
volatile int n;
volatile int rem;
int* arr;


DWORD WINAPI marker(LPVOID nu) {
	WaitForSingleObject(hStartEvent, INFINITE);
	
	int a, k=0, num = (int)nu;
	srand(num);

	while (true) {
		a = rand();
		a %= n;
		EnterCriticalSection(&cs);
		if (arr[a] == 0) {
			Sleep(5);
			arr[a] = num;
			k++;
			Sleep(5);
			
		}
		else {
			cout << num << " " << k << " " << a << endl;
			LeaveCriticalSection(&cs);

			SetEvent(hFinishEvent[num-1]);
			WaitForSingleObject(hRemoveEvent, INFINITE);

			if (rem == int(num)) {
				for (int i = 0; i < n; i++) {
					if (arr[i] == num) arr[i] = 0;
				}
				break;
			}
			else {
				ResetEvent(hFinishEvent[num - 1]);
				WaitForSingleObject(hContinueEvent, INFINITE);
			}
		}
		LeaveCriticalSection(&cs);
	}

	SetEvent(hFinishEvent[(int)num - 1]);
	return 0;
}

int main()
{
	DWORD* IDMarkers;
	HANDLE* hMarkers;
	InitializeCriticalSection(&cs);

	int m, a;
	cout << "Type your array size: ";
	cin >> a;
	n = a;
	arr = new int[n];
	ZeroMemory(arr, sizeof(int) * n);

	cout << "Type number of markers: ";
	cin >> m;
	hMarkers = new HANDLE[m];
	IDMarkers = new DWORD[m];
	hFinishEvent = new HANDLE[m];

	hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hStartEvent == NULL) {
		return GetLastError();
	}
	hContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hContinueEvent == NULL) {
		return GetLastError();
	}
	hRemoveEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hRemoveEvent == NULL) {
		return GetLastError();
	}

	for (int i = 0; i < m; i++) {
		hMarkers[i] = CreateThread(NULL, 0, marker, (LPVOID)(i+1), 0, &IDMarkers[i]);
		if (hMarkers[i] == NULL)
			return GetLastError();
		hFinishEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hFinishEvent[i] == NULL)
			return GetLastError();
	}

	SetEvent(hStartEvent);

	int number = m, end;
	while (number > 0) {
		if (WaitForMultipleObjects(m, hFinishEvent, TRUE, INFINITE) == WAIT_FAILED)
		{
			cout << "Wait for multiple objects failed." << endl;
			cout << "Press any key to exit." << endl;
		}
		EnterCriticalSection(&cs);
		for (int i = 0; i < n; i++) {
			cout << arr[i] << " ";
		}
		LeaveCriticalSection(&cs);
		cout << "\nType a number of marker thar you wanna delete: ";
		cin >> end;
		rem = end;
		PulseEvent(hRemoveEvent);
		WaitForSingleObject(hMarkers[rem-1], INFINITE);
		number--;
		PulseEvent(hContinueEvent);
	}

	for (int i = 0; i < m; i++) {
		CloseHandle(hMarkers[i]);
		CloseHandle(hFinishEvent[i]);
	}
	
	CloseHandle(hStartEvent);
	DeleteCriticalSection(&cs);

	return 0;
}