//#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;

struct Employee {
	int num;
	char name[10];
	double hours;
};

Employee* emps;
int empsNum;
HANDLE hSemaphore, hMutex;
int readerCount = 0;

DWORD WINAPI InstanceThread(LPVOID param) {
	HANDLE hPipe = (HANDLE)param;
	int empNum, fEmpNum;
	char command[20] = "";
	DWORD dwBytesRead, dwBytesWrote;
	BOOL fSuccess = FALSE;
	bool found = false;
	while (true) {
		fSuccess = ReadFile(hPipe, &command, sizeof(command), &dwBytesRead, NULL);
		if (!fSuccess || dwBytesRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE)
				printf("InstanceThread: client disonnected.\n");
			else
				printf("IntanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
			break;
		}

		if (strcmp(command, "read") == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			readerCount++;
			if (readerCount == 1)
				WaitForSingleObject(hSemaphore, INFINITE);
			ReleaseMutex(hMutex);

			fSuccess = ReadFile(hPipe, &empNum, sizeof(empNum), &dwBytesRead, NULL);
			if (!fSuccess || dwBytesRead == 0) {
				printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
				break;
			}
			
			found = false;
			for (int i = 0; i < empsNum; i++) {
				if (emps[i].num == empNum) {
					found = true;
					fEmpNum = i;
				}
			}

			fSuccess = WriteFile(hPipe, &found, sizeof(bool), &dwBytesWrote, NULL);
			if (!fSuccess || dwBytesWrote == 0) {
				printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
				break;
			}

			if (found) {
				fSuccess = WriteFile(hPipe, &emps[fEmpNum], sizeof(Employee), &dwBytesWrote, NULL);
				if (!fSuccess || dwBytesWrote == 0) {
					printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
					break;
				}
			}

			WaitForSingleObject(hMutex, INFINITE);
			readerCount--;
			if (readerCount == 0)
				ReleaseSemaphore(hSemaphore, 1, NULL);
			ReleaseMutex(hMutex);
		}
		else if (strcmp(command, "write") == 0) {
			WaitForSingleObject(hSemaphore, INFINITE);
			fSuccess = ReadFile(hPipe, &empNum, sizeof(empNum), &dwBytesRead, NULL);
			if (!fSuccess || dwBytesRead == 0) {
				printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
				break;
			}

			found = false;
			for (int i = 0; i < empsNum; i++) {
				if (emps[i].num == empNum) {
					found = true;
					fEmpNum = i;
				}
			}

			fSuccess = WriteFile(hPipe, &found, sizeof(bool), &dwBytesWrote, NULL);
			if (!fSuccess || dwBytesWrote == 0) {
				printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
				break;
			}

			if (found) {
				fSuccess = WriteFile(hPipe, &emps[fEmpNum], sizeof(Employee), &dwBytesWrote, NULL);
				if (!fSuccess || dwBytesWrote == 0) {
					printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
					break;
				}

				fSuccess = ReadFile(hPipe, &emps[fEmpNum], sizeof(Employee), &dwBytesRead, NULL);
				if (!fSuccess || dwBytesRead == 0) {
					printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
					break;
				}
			}

			ReleaseSemaphore(hSemaphore, 1, NULL);
		}
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	printf("Thread ended his work.\n");
	return 1;
}

int main()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO cb;
	wchar_t commandLine[] = L"Client.exe";
	int clientsNum, readerCount = 0;

	printf("Type number of employees: ");
	scanf_s("%d", &empsNum);
	emps = new Employee[empsNum];

	printf("Type your employees (num, name, hours):\n");
	for (int i = 0; i < empsNum; i++) {
		printf("%d)", i+1);
		scanf_s("%d %s %lf", &emps[i].num, &emps[i].name, 20, &emps[i].hours);
	}

	printf("Type number of clients: ");
	scanf_s("%d", &clientsNum);

	hSemaphore = CreateSemaphore(NULL, 1, clientsNum, L"WriteSempahore");
	hMutex = CreateMutex(NULL, FALSE, L"SyncMutex");

	HANDLE hPipe;
	HANDLE* hThreads;
	hThreads = new HANDLE[clientsNum];
	DWORD dwThreadId;
	for (int i = 0; i < clientsNum; i++) {
		hPipe = CreateNamedPipe(
			L"\\\\.\\pipe\\server_pipe",
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			clientsNum, 0, 0,
			INFINITE,
			NULL);
		if (hPipe == INVALID_HANDLE_VALUE) {
			printf("Pipe %d creation failed, GLE: %d.\n", i + 1, GetLastError());
			return -1;
		}

		ZeroMemory(&cb, sizeof(STARTUPINFO));
		cb.cb = sizeof(STARTUPINFO);
		if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
			NULL, NULL, &cb, &pi)) {
			printf("Process %d creation failed.\n", i + 1);
			break;
		}
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		if (ConnectNamedPipe(hPipe, NULL)) {
			printf("Client %d connected.\n", i + 1);
			hThreads[i] = CreateThread(NULL, 0, InstanceThread, (LPVOID)hPipe, 0, &dwThreadId);
			if (hThreads[i] == 0) {
				printf("Thread %d creation failed.\n", i+1);
				return -1;
			}
		}
		else {
			CloseHandle(hPipe);
		}
	}

	WaitForMultipleObjects(clientsNum, hThreads, TRUE, INFINITE);
	for (int i = 0; i < clientsNum; i++) {
		CloseHandle(hThreads[i]);
	}
	return 0;
}
