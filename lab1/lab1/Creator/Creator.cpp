#include <conio.h>
#include <fstream>
#include <iostream>
#include <stdio.h>


using namespace std;

struct employee
{
	int num;
	char name[10];
	double hours;

};

int main(int argc, char* argv[])
{
	int numRec = atoi(argv[2]);
	employee *workersList = new employee[numRec];

	fstream binFile(argv[1], ios::binary | ios::out);
	cout << "Type a list of employeers: " << endl;
	for (int i = 0; i < numRec; i++) {
		cout << i + 1 << ") ";
		cin >> workersList[i].num >> workersList[i].name >> workersList[i].hours;
	}

	for (int i = 0; i < numRec; i++) {
		binFile.write((char*)&workersList[i], sizeof(employee));
	}

	cout << "Press Enter to end the program..." << endl;
	_getch();

	binFile.close();

	return 0;
}

