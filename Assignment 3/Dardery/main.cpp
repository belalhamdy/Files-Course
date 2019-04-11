#include <bits/stdc++.h>
#include "primaryIndicies.h"
#include "secondryIndicies.h"

using namespace std;


fstream dataFile;

PrimaryIndicies primaryIndex;
SecondryIndicies secondryIndex;
void addCourse(const Course& c);
void deleteSingleCourse(int offset, bool deleteSecondry);
void updateCourse(int offset, const Course& c);

void cinCourse(Course&);
void coutCourse(const Course&);
int cinID(char[6]);
int cinInstructor(string&);

void atStartUtil();
void addCourseUtil();
void deleteCourseIDUtil();
void deleteCourseInstructorUtil();
void printCourseIDUtil();
void printCourseInstructorUtil();
void updateCourseIDUtil();
void updateCourseInstructorUtil();
void atEndUtil();

int main() {
	atStartUtil();

	int c = -1;
	while (c) {
		cout <<
			"1) Add New course\n" <<
			"2) Delete course (by ID)\n" <<
			"3) Delete course (by instructor)\n" <<
			"4) Print course (by ID)\n" <<
			"5) Print courses (by instructor)\n" <<
			"6) Update course (by ID)\n" <<
			"7) Update first course (by instructor)\n" <<
			"0) Exit\n" <<

			"DEBUGGING\n"<<
			"10) Erase all files\n" <<
			"11) reconstruct indicies\n" <<
			"Please Enter Your Choice: ";

		cin >> c;
		cin.ignore();
		switch (c) {
		case 1:
			addCourseUtil();
			break;
		case 2:
			deleteCourseIDUtil();
			break;
		case 3:
			deleteCourseInstructorUtil();
			break;
		case 4:
			printCourseIDUtil();
			break;
		case 5:
			printCourseInstructorUtil();
			break;
		case 6:
			updateCourseIDUtil();
			break;
		case 7:
			updateCourseInstructorUtil();
			break;
		case 10: {

			dataFile.close();
			//invertedList.close();
			ofstream temp;
			temp.open("data.txt", ios::trunc);
			temp.close();
			//temp.open("inverted.txt", ios::trunc);
			//temp.close();
			temp.open(primaryIndex.FILE_NAME, ios::trunc);
			temp.close();
			temp.open(secondryIndex.FILE_NAME, ios::trunc);
			temp.close();
			atStartUtil();
			break;
		}
		case 11:
			primaryIndex.setStatus(0);
			secondryIndex.setStatus(0);
			dataFile.close();
			//invertedList.close();
			atStartUtil();
			break;
		case 0:
			break;
		default:
			cout << "Invalid choice\n";
			break;
		}

	}
	atEndUtil();
}


void coutCourse(const Course& c) {
	cout << "\n";
	cout << "Course ID  : " << c.ID << "\n";
	cout << "Course Name: " << c.name << "\n";
	cout << "Instructor : " << c.instructor << "\n";
	cout << "Duration   : " << c.weeks << "\n";
	cout << "------------------------------------\n";
}
void cinCourse(Course& c) {
	cout << "Enter ID: ";
	cin.getline(c.ID, sizeof c.ID);
	cout << "Enter Course Name: ";
	getline(cin, c.name);
	cout << "Enter Instructor Name: ";
	getline(cin, c.instructor);
	cout << "Enter Course duration in weeks: ";
	cin >> c.weeks;
}
int cinID(char id[6]) {
	cout << "Enter ID: ";
	cin.getline(id, 6 * sizeof id[0]);
	int i = primaryIndex.findIndexFor(id);
	if (i == -1) {
		cout << "ID not found!\n";
	}
	return i;
}
int cinInstructor(string& inst) {
	cout << "Enter Instructor: ";
	getline(cin, inst);
	int i = secondryIndex.findIndexFor(inst);
	if (i == -1) {
		cout << "Instructor not found!\n";
	}
	return i;
}

void openFile(fstream& file, const char* filename) {
	file.open(filename, ios::in | ios::out | ios::binary);
	if (!file.is_open()) {
		file.open(filename, ios::out | ios::binary);
		file.close();
		file.open(filename, ios::in | ios::out | ios::binary);
		if (!file.is_open()) exit(0);
	}
}

void atStartUtil()
{
	openFile(dataFile, "data.txt");
	primaryIndex.fetchPrimary(dataFile);
	secondryIndex.fetchSecondry(dataFile);
}

void addCourseUtil()
{
	Course c;
	cinCourse(c);
	if (c.ID[0] == 0) return;

	addCourse(c);
}

void deleteCourseIDUtil()
{
	char id[6];
	int i = cinID(id);
	if (~i) {	//anything by -1
		deleteSingleCourse(primaryIndex[i].offset, true);
	}
}

void deleteCourseInstructorUtil()
{
	string instructor;
	int i = cinInstructor(instructor);
	if (~i) {	//anything by -1

		int nxt = secondryIndex[i].labelID;
		char id[6];
		while (nxt != -1) {
			secondryIndex.readInvertedItem(nxt, nxt, id);
	
			int j = primaryIndex.findIndexFor(id);
			assert(j != -1);

			Course c;
			deleteSingleCourse(primaryIndex[j].offset, false);
		}
		secondryIndex.removeIndexAt(i);
	}
}

void printCourseIDUtil()
{
	char id[6];
	int i = cinID(id);
	if (~i) {	//anything by -1
		Course c;
		readCourse(primaryIndex[i].offset, dataFile, c);
		coutCourse(c);
	}
}

void printCourseInstructorUtil()
{
	string instructor;
	int i = cinInstructor(instructor);
	if (~i) {	//anything by -1
		int nxt = secondryIndex[i].labelID;
		char id[6];
		while (nxt != -1) {
			secondryIndex.readInvertedItem(nxt, nxt, id);

			int j = primaryIndex.findIndexFor(id);
			assert(j != -1);

			Course c;
			readCourse(primaryIndex[j].offset, dataFile, c);
			coutCourse(c);
		}
	}
}

void updateCourseIDUtil()
{
	char id[6];
	int i = cinID(id);
	if (~i) {	//anything by -1
		cout << "Enter new course:\n";
		Course c;
		cinCourse(c);
		updateCourse(primaryIndex[i].offset, c);
	}
}

void updateCourseInstructorUtil()
{
	string instructor;
	int i = cinInstructor(instructor);
	if (~i) {	//anything by -1
		int nxt = secondryIndex[i].labelID;
		assert(nxt != -1);
		char id[6];
		secondryIndex.readInvertedItem(nxt, nxt, id);

		int j = primaryIndex.findIndexFor(id);
		assert(j != -1);

		cout << "Enter new course information for course " << id << " :\n";
		Course c;
		cinCourse(c);
		updateCourse(primaryIndex[j].offset, c);
	}
	return;

}

void atEndUtil()
{
	dataFile.close();
	primaryIndex.flushPrimary();
	secondryIndex.flushSecondry();
}


void addCourse(const Course& c) {
	dataFile.seekp(0, ios::end);
	int offset = dataFile.tellp();

	writeCourse(-1, dataFile, c);

	primaryIndex.insertIndexFor(c, offset);
	secondryIndex.insertIndexFor(c);

}

void deleteSingleCourse(int offset, bool deleteSecondry) {
	Course c;
	readCourse(offset, dataFile, c);

	//seek to the begining of the ID.
	dataFile.seekp(offset + sizeof short(-1));
	dataFile.put(DELETE_MARK);

	primaryIndex.removeIndexFor(c);

	if (deleteSecondry) {
		secondryIndex.removeIndexFor(c);
	}
}

void updateCourse(int offset, const Course& c) {
	deleteSingleCourse(offset, true);
	addCourse(c);
}

/*
1
aaa
ahmed
ahmed
50
1
bbb
nasr
nasr
100
1
ccc
dardery
dardery
150
1
ddd
ahmed2
ahmed
200
1
eee
ahmed3
ahmed
250
1
fff
nasr2
nasr
300
1
ggg
ibrahim
ibrahim
350
*/