// Пример процесса, который создает новое консольное приложение с новой консолью

#include <windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;


struct employee
{
	int num;
	char name[10];
	double hours;

};

int main()
{
	wchar_t lpszCommandLine[50];
	wstring binFileName;
	int numRec;
	cout << "Type a name for binary file and number of records: " << endl;
	wcin >> binFileName >> numRec;
	wstring s  = L"Creator.exe " + binFileName + L" " + to_wstring(numRec);
	lstrcpyW(lpszCommandLine, s.data());
	

	STARTUPINFO si;
	PROCESS_INFORMATION piCreator;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, lpszCommandLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &piCreator)) {
		cout << "The new process didn`t created. Check the name of file" << endl;
		cout << "Press any key to exit...";
		_getch();
		return 1;
	}

	cout << "Process Creator was started" << endl;

	WaitForSingleObject(piCreator.hProcess, INFINITE);
	CloseHandle(piCreator.hThread);
	CloseHandle(piCreator.hProcess);
	
	cout << "\nProcess Creator had been worked and made this file: " << endl;

	ifstream binFile(binFileName, ios::binary);
	employee worker;
	for (int i = 0; i < numRec; i++) {
		binFile.read((char*)&worker, sizeof(employee));
		cout << worker.num << " " << worker.name << " " << worker.hours << endl;
	}
	
	

	float payHour;
	wstring reportName;
	cout << "\n\nType a name for report file and payment for an hour of work: " << endl;
	wcin >> reportName >> payHour;
	s = L"Reporter.exe " + binFileName + L" " + reportName + L" " + to_wstring(payHour);
	wcout << s << endl;
	lstrcpyW(lpszCommandLine, s.data());


	PROCESS_INFORMATION piReporter;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, lpszCommandLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &piReporter)) {
		cout << "The new process didn`t created. Check the name of file" << endl;
		cout << "Press any key to exit...";
		_getch();
		return 1;
	}

	cout << "Process Reporter was started" << endl;

	WaitForSingleObject(piReporter.hProcess, INFINITE);
	CloseHandle(piReporter.hThread);
	CloseHandle(piReporter.hProcess);

	cout << "\nProcess Reporter had been worked and made this text file: " << endl;
	ifstream report(reportName);
	string str;
	while (getline(report, str)) {
		cout << str << endl;
	}

	cout << "Press Enter to leave the program..." << endl;
	_getch();
	return 0;
}
