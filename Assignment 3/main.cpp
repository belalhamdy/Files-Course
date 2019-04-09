#include <bits/stdc++.h>
using namespace std;

const char DELIM = '|';

fstream filedata;

struct Course {
	char ID[6];
	string name;
	string instructor;
	short weeks;
};

struct Primary {
	char ID[6];
	int offset;
	bool operator<(const Primary & b) {
		int cmp = strcmp(ID, b.ID);
		if (cmp != 0)
			return cmp < 0;
		else
			return offset < b.offset;
	}
	bool operator==(const Primary & b) {
		return strcmp(ID, b.ID) == 0;
	}
};

struct Secondry {
	string key;
	int labelID;
	bool operator<(const Secondry & b) {
		return key < b.key;
	}
	bool operator==(const Secondry & b) {
		return key == b.key;
	}
 };

vector<Primary> primaryIdx;
vector<Secondry> secondryIdx;

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


void openFile(fstream& file, const char* filename) {
	file.open(filename, ios::in | ios::out);
	if (!file.is_open()) {
		file.open(filename, ios::out);
		file.close();
		file.open(filename, ios::in | ios::out);
		if (!file.is_open()) exit(0);
	}
}

void writeCourse(const Course& c) {
	short len = sizeof c.ID - 1 + c.name.length() + c.instructor.length() + sizeof c.weeks + 4;

	filedata.write((char*)&len, sizeof len);
	filedata.write(c.ID, sizeof c.ID - 1);
	filedata.put('|');
	filedata.write(c.name.c_str(), c.name.length());
	filedata.put('|');
	filedata.write(c.instructor.c_str(), c.instructor.length());
	filedata.put('|');
	filedata.write((char*)&c.weeks, sizeof c.weeks);
	filedata.put('|');

}
void readCourse(Course& c) {
	short len;

	filedata.read((char*)&len, sizeof len);
	filedata.getline(c.ID, sizeof c.ID, '|');
	getline(filedata, c.name, '|');
	getline(filedata, c.instructor, '|');
	filedata.read((char*)&c.weeks, sizeof c.weeks);
	filedata.get();

}
int updateCourse(const Course& c) {
	short oldlen, newlen;
	newlen = sizeof c.ID - 1 + c.name.length() + c.instructor.length() + sizeof c.weeks + 4;
	filedata.read((char*)&oldlen, sizeof oldlen);
	
	filedata.seekp((int)filedata.tellg() - sizeof oldlen);

	int offset = filedata.tellp();
	if (oldlen < newlen) {
		filedata.put('*');
		filedata.seekp(0, ios::end);
		offset = filedata.tellp();
		writeCourse(c);
	}
	else {
		writeCourse(c);
	}
	return offset;
}

void coutCourse(const Course& c) {
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


bool getStatus(const char* filename) {
	ifstream file(filename);
	char value = file.get();
	return value == 0 ? 1 : 0;
	file.close();
}
void setStatus(const char* filename, bool uptodate) {
	char value = uptodate ? 0 : -1;
	ofstream file(filename);
	file.seekp(0);
	file.put(value);
	file.close();
}


//fixed-length fields and fixed-length records
void reconstructPrimary() {
	int offset = 0;
	filedata.seekg(offset);
	primaryIdx.clear();
	while (filedata.peek() != EOF) {
		offset = filedata.tellg();

		Course course;
		readCourse(course);

		Primary idx;
		strcpy(idx.ID, course.ID);
		idx.offset = offset;

		primaryIdx.push_back(idx);
	}
	filedata.clear();
	sort(primaryIdx.begin(), primaryIdx.end());
}
void fetchPrimary() {
	bool uptodate = getStatus("primary.txt");
	if (!uptodate) {
		reconstructPrimary();
		return;
	}
	primaryIdx.clear();
	ifstream prim("primary.txt");
	prim.seekg(1);
	while (prim.peek() != EOF) {
		Primary cur;
		prim.read(cur.ID, sizeof cur.ID);
		prim.read((char*)&cur.offset, sizeof cur.offset);
			
		primaryIdx.push_back(cur);
	}

	prim.close();
}
void flushPrimary() {
	ofstream prim("primary.txt");
	char status = 0;
	prim.put(status);
	for (Primary cur : primaryIdx) {
		prim.write(cur.ID, sizeof cur.ID);
		prim.write((char*)&cur.offset, sizeof cur.offset);
	}
	prim.close();
}

//fixed-length and fixed-length records
void reconstructSecondry() {
	int offset = 0;
	filedata.seekg(offset);
	secondryIdx.clear();
	while (filedata.peek() != EOF) {
		offset = filedata.tellg();

		Course course;
		readCourse(course);

		Secondry idx;
		idx.key = course.instructor;
		strcpy(idx.ID, course.ID);

		secondryIdx.push_back(idx);
	}
	filedata.clear();
	sort(secondryIdx.begin(), secondryIdx.end());
}
void fetchSecondry() {
	bool uptodate = getStatus("secondry.txt");
	if (!uptodate) {
		reconstructSecondry();
		return;
	}
	secondryIdx.clear();
	ifstream sec("secondry.txt");
	sec.seekg(1);
	while (sec.peek() != EOF) {
		Secondry cur;
		getline(sec, cur.key, '|');
		sec.read((char*)&cur.labelID, sizeof cur.labelID);

		secondryIdx.push_back(cur);
	}
	sec.close();
}
void flushSecondry() {
	ofstream sec("secondry.txt");
	char status = 0;
	sec.put(status);
	for (Secondry cur : secondryIdx) {
		sec << cur.key << '|';
		sec.write((char*)&cur.labelID, sizeof cur.labelID);
	}
	sec.close();
}

void atStartUtil();
void addCourseUtil();
void deleteCourseIDUtil();
void deleteCourseInstructorUtil();
void printCourseIDUtil();
void printCourseInstructorUtil();
void updateCourseIDUtil();
void updateCourseInstructorUtil();
void atEndUtil();

int maint() {
	atStartUtil();

	while (1) {
		cout <<
			"1) Add New course\n" <<
			"2) Delete course (by ID)\n" <<
			"3) Delete course (by instructor)\n" <<
			"4) Print course (by ID)\n" <<
			"5) Print course (by instructor)\n" <<
			"6) update course (by ID)\n" <<
			"7) update course (by instructor)\n" <<
			"8) Exit\n" <<
			"Please Enter Your Choice: ";

		int c;
		cin >> c;
		switch (c) {
		case 1:
			addCourseUtil();
		case 2:
			deleteCourseIDUtil();
		case 3:
			deleteCourseInstructorUtil();
		case 4:
			printCourseIDUtil();
		case 5:
			printCourseInstructorUtil();
		case 6:
			updateCourseIDUtil();
		case 7:
			updateCourseInstructorUtil();
		case 8:
			break;
		default:
			cout << "Invalid choice\n";
		}

	}
	atEndUtil();
}

void atStartUtil()
{
	openFile(filedata, "data.txt");
	fetchPrimary();
	fetchSecondry();
}

void addCourseUtil()
{
	Course c;
	cinCourse(c);
	filedata.seekp(0, ios::end);

	Primary idx;
	strcpy(idx.ID, c.ID);
	idx.offset = filedata.tellp();

	primaryIdx.insert(upper_bound(primaryIdx.begin(), primaryIdx.end(), idx), idx);

	writeCourse(c);
}

void deleteCourseIDUtil()
{
	Primary idx;
	cout << "Enter ID: ";
	cin.getline(idx.ID, sizeof idx.ID);
	int i = binarySearch(primaryIdx, idx);
	if (i == -1) {
		cout << "ID not found!\n";
	}
	else {
		int offset = primaryIdx[i].offset;
		primaryIdx.erase(primaryIdx.begin() + i);
		setStatus("primary.txt", 0);
		filedata.seekp(offset);
		filedata.put('*');
	}

}

void deleteCourseInstructorUtil()
{
}

void printCourseIDUtil()
{
	Primary idx;
	cout << "Enter ID: ";
	cin.getline(idx.ID, sizeof idx.ID);
	int i = binarySearch(primaryIdx, idx);
	if (i == -1) {
		cout << "ID not found!\n";
	}
	else {
		int offset = primaryIdx[i].offset;
		filedata.seekg(offset);
		Course c;
		readCourse(c);
		coutCourse(c);
	}
}

void printCourseInstructorUtil()
{
	Secondry idx;
	cout << "Enter Instructor: ";
	getline(cin, idx.key);
	int i = binarySearch(secondryIdx, idx);
	if (i == -1) {
		cout << "Instructor not found!\n";
	}
	else {
		Primary idx2;
		strcpy(idx2.ID, secondryIdx[i].ID);
		int j = binarySearch(primaryIdx, idx2);
		assert(j != -1);
		int offset = primaryIdx[j].offset;
		filedata.seekg(offset);
		Course c;
		readCourse(c);
		coutCourse(c);
	}
}

void updateCourseIDUtil()
{
	Primary idx;
	cout << "Enter ID: ";
	cin.getline(idx.ID, sizeof idx.ID);
	int i = binarySearch(primaryIdx, idx);
	if (i == -1) {
		cout << "ID not found!\n";
	}
	else {
		int offset = primaryIdx[i].offset;
		Course c;
		cinCourse(c);
		
		filedata.seekp(offset);
		filedata.seekg(offset);
		int off = updateCourse(c);
		primaryIdx[i].offset = off;
		strcpy(primaryIdx[i].ID, c.ID);

		//pretty much just a sort
		while (i + 1 < (int)primaryIdx.size() && primaryIdx[i + 1] < primaryIdx[i]) {
			swap(primaryIdx[i + 1], primaryIdx[i]);
		}
		while (i - 1 >= 0 && primaryIdx[i] < primaryIdx[i - 1]) {
			swap(primaryIdx[i - 1], primaryIdx[i]);
		}
	}
}

void updateCourseInstructorUtil()
{
}

void atEndUtil()
{
	filedata.close();
	flushPrimary();
	flushSecondry();
}
