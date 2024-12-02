#include <iostream>
#include <windows.h>

using namespace std;

struct Employee {
	int num;
	char name[10];
	double hours;
};

int main()
{
	HANDLE hProcess;
	wchar_t commandLine[] = L"Client.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	int data;
	char command[20];
	DWORD dw;

	Employee emp;
	Employee* emps;
	int empsNum;

	cout << "Type number of employees: ";
	cin >> empsNum;
	emps = new Employee[empsNum];
	cout << "Type your employees (num, name, hours):" << endl;
	for (int i = 0; i < empsNum; i++) {
		cout << i+1 << ") ";
		cin >> emps[i].num >> emps[i].name >> emps[i].hours;
	}

	HANDLE hServer = CreateNamedPipe(
		L"\\\\.\\pipe\\server_pipe",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_WAIT,
		1,
		0,
		0,
		INFINITE,
		(LPSECURITY_ATTRIBUTES)NULL
	);
	if (hServer == INVALID_HANDLE_VALUE) {
		cout << "Pipe creation failed";
		return 1;
	}

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
		NULL, NULL, &si, &pi)) {
		cout << "Process creatio failed" << endl;
		CloseHandle(hServer);
		return 1;
	}

	cout << "server is awaiting for the client..." << endl;
	if (!ConnectNamedPipe(hServer, NULL)) {
		cout << "Connection error with code: " << GetLastError << endl;
		CloseHandle(hServer);
		return 1;
	}

	while (true) {

		if (!ReadFile(hServer, &command, sizeof(command), &dw, NULL)) {
			cerr << "Reading from pipe error";
			CloseHandle(hServer);
			return 1;
		}
		cout << "Server get " << command << endl;

		if (strcmp(command, "exit") == 0)
			break;

		else if (strcmp(command, "read") == 0) {
			if (!ReadFile(hServer, &data, sizeof(data), &dw, NULL)) {
				cerr << "Reading from pipe error";
				CloseHandle(hServer);
				return 1;
			}

			bool found = false;
			for (int i = 0; i < empsNum; i++) {
				if (data == emps[i].num) {
					emp = emps[i];
					found = true;
				}
			}

			if(found == false) {
				emp.num = 0; strcpy_s(emp.name, "NOT FOUND"); emp.hours = 0;
			}
			if (!WriteFile(hServer, &emp, sizeof(emp), &dw, NULL)) {
				cerr << "Writing in pipe error" << endl;
				return 1;
			}
		}
	}
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hServer);
	return 0;
}
