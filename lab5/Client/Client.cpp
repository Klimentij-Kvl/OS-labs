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
    char c[20];
    char command[20];
    int data;
    Employee emp;
    DWORD dw;
    bool b;

    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"WriteSemaphore");
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"SyncMutex");
    HANDLE hServer = CreateFileA(
        "\\\\.\\pipe\\server_pipe",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING,
        0,
        (HANDLE)NULL
    );
    if (hServer == INVALID_HANDLE_VALUE) {
        cout << "Opening pipe error" << endl << "Type any char to exit";
        cin >> c;
        return 1;
    }


    while (true) {
        cout << "Type a command(read, write or exit): ";
        cin >> command;
        if (strcmp(command, "exit") == 0) {
            break;
        }
        if (!WriteFile(hServer, &command, sizeof(command), &dw, NULL)) {
            cerr << "Writing in pipe error" << endl << "Type any char to exit";
            cin >> c;
            return 1;
        }


        else if (strcmp(command, "read") == 0) {
            cout << "Type number of employee: ";
            cin >> data;
            if (!WriteFile(hServer, &data, sizeof(data), &dw, NULL)) {
                cerr << "Writing in pipe error" << endl << "Type any char to exit";
                cin >> c;
                return 1;
            }

            if (!ReadFile(hServer, &b, sizeof(bool), &dw, NULL)) {
                cerr << "Reading from pipe error";
                CloseHandle(hServer);
                return 1;
            }

            if (b == false) {
                cout << "Employee not found" << endl;
                continue;
            }
            else {
                if (!ReadFile(hServer, &emp, sizeof(emp), &dw, NULL)) {
                    cerr << "Reading from pipe error";
                    CloseHandle(hServer);
                    return 1;
                }
                cout << "Client got this emloyee: " << emp.num << " " << emp.name
                    << " " << emp.hours << endl;
            }
            cout << "type any key to end reading...";
            cin >> c;
        }
        else if (strcmp(command, "write") == 0) {
            cout << "Type number of employee: ";
            cin >> data;
            if (!WriteFile(hServer, &data, sizeof(data), &dw, NULL)) {
                cerr << "Writing in pipe error" << endl << "Type any char to exit";
                cin >> c;
                return 1;
            }

            if (!ReadFile(hServer, &b, sizeof(bool), &dw, NULL)) {
                cerr << "Reading from pipe error";
                CloseHandle(hServer);
                return 1;
            }

            if (b == false) {
                cout << "Employee not found" << endl;
                continue;
            }
            else {
                if (!ReadFile(hServer, &emp, sizeof(emp), &dw, NULL)) {
                    cerr << "Reading from pipe error";
                    CloseHandle(hServer);
                    return 1;
                }
                cout << "Client got this emloyee: " << emp.num << " " << emp.name
                    << " " << emp.hours << endl;
                cout << "Type new employee (number, name, hours): ";
                cin >> emp.num >> emp.name >> emp.hours;

                cout << "Type something to send this employee to a server: ";
                cin >> c;

                if (!WriteFile(hServer, &emp, sizeof(emp), &dw, NULL)) {
                    cerr << "Writing in pipe error" << endl << "Type any char to exit";
                    cin >> c;
                    return 1;
                }
            }

            cout << "Type comething to end the writing: ";
            cin >> c;
        }
    }

    CloseHandle(hServer);
    cout << "Type any key to leave the program";
    cin >> c;
    return 0;
}