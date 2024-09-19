// Консольный процесс, который выводит на консоль свое имя и параметры

#include <conio.h>
#include <fstream>
#include <vector>


using namespace std;

struct employee
{
	int num; // идентификационный номер сотрудника
	char name[10]; // имя сотрудника
	double hours; // количество отработанных часов
};

int compare(const void *x1, const void *x2) {
	employee* worker1 = (employee*)x1;
	employee* worker2 = (employee*)x2;

	return worker1->num - worker2->num;
}

int main(int argc, char* argv[])
{
	ifstream binFile(argv[1], ios::binary);
	ofstream report(argv[2]);

	float payHour = atof(argv[3]);
	vector<employee> workersList;
	employee worker;

	while (binFile.read((char*)&worker, sizeof(employee))) {
		workersList.push_back(worker);
	}

	report << "Report on file " << argv[0] << endl;

	report << "num name hours salary" << endl;

	qsort(workersList.data(), workersList.size(), sizeof(employee), compare);

	for (int i = 0; i < workersList.size(); i++) {
		report << workersList[i].num << " " << workersList[i].name << " " << workersList[i].hours << " "
			<< workersList[i].hours * payHour << endl;
	}

	_cputs("Report was created successfully.\n");

	_cputs("Press Enter to leave the program...\n");
	_getch();

	return 0;
}