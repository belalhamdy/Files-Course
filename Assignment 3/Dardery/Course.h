#ifndef _COURSE_H
#define _COURSE_H

#include <string>
#include <fstream>

using namespace std;

#define EMPTYID { ' ',' ',' ',' ',' ',' ' }

const char DELETE_MARK = '*';
const char DELIM_MARK = '|';

struct Course {
	char ID[6] = EMPTYID;
	string name;
	string instructor;
	short weeks;
};

template<typename T>
int binarySearch(vector<T> vec, T val) {

	int sz = vec.size();
	int st = 0, en = sz - 1;
	while (st <= en)
	{
		int mid = (st + en) / 2;
		if (vec[mid] == val)
			return mid;
		else if (vec[mid] < val)
			st = mid + 1;
		else
			en = mid - 1;
	}
	return -1;
}


bool getStatus(const char* filename) {
	ifstream file(filename, ios::binary);
	if (!file.is_open()) return 0;
	char value = file.get();
	file.close();
	return value == 0 ? 1 : 0;
}
void setStatus(const char* filename, bool uptodate) {
	char value = uptodate ? 0 : -1;
	ofstream file(filename, ios::binary);
	file.seekp(0);
	file.put(value);
	file.close();
}


void readCourse(int offset,fstream& dataFile, Course& c) {
	short len;

	if (offset != -1)
		dataFile.seekg(offset);

	dataFile.read((char*)&len, sizeof len);
	dataFile.getline(c.ID, sizeof c.ID, '|');
	getline(dataFile, c.name, '|');
	getline(dataFile, c.instructor, '|');
	dataFile.read((char*)&c.weeks, sizeof c.weeks);
	dataFile.get();

}
void writeCourse(int offset, fstream& dataFile, const Course& c) {
	short len = sizeof c.ID - 1 + c.name.length() + c.instructor.length() + sizeof c.weeks + 4;
	
	if (offset != -1)
		dataFile.seekp(offset);

	dataFile.write((char*)&len, sizeof len);
	dataFile.write(c.ID, sizeof c.ID - 1);
	dataFile.put('|');
	dataFile.write(c.name.c_str(), c.name.length());
	dataFile.put('|');
	dataFile.write(c.instructor.c_str(), c.instructor.length());
	dataFile.put('|');
	dataFile.write((char*)&c.weeks, sizeof c.weeks);
	dataFile.put('|');

}

#endif