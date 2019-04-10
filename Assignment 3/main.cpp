#include <bits/stdc++.h>
using namespace std;

const char DELIM = '|';
#define EMPTYID { ' ',' ',' ',' ',' ',' ' }
fstream dataFile;

//fixed-length fields (int nxt, char ID[6])
fstream invertedList;

struct Course {
	char ID[6] = EMPTYID;
	string name;
	string instructor;
	short weeks;
};

struct Primary {
	char ID[6] = EMPTYID;
	int offset;
	bool operator<(const Primary & b) const {
		int cmp = strcmp(ID, b.ID);
		if (cmp != 0)
			return cmp < 0;
		else
			return offset < b.offset;
	}
	bool operator==(const Primary & b) const {
		return strcmp(ID, b.ID) == 0;
	}
};

struct Secondry {
	string key;
	int labelID;
	bool operator<(const Secondry & b) const {
		return key < b.key;
	}
	bool operator==(const Secondry & b) const {
		return key == b.key;
	}
};

const int INVERTED_SIZE = sizeof(-1) + sizeof Course::ID;

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
	file.open(filename, ios::in | ios::out | ios::binary);
	if (!file.is_open()) {
		file.open(filename, ios::out | ios::binary);
		file.close();
		file.open(filename, ios::in | ios::out | ios::binary);
		if (!file.is_open()) exit(0);
	}
}
void writeCourse(const Course& c) {
	short len = sizeof c.ID - 1 + c.name.length() + c.instructor.length() + sizeof c.weeks + 4;

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
void addCourse(const Course& c) {
	dataFile.seekp(0, ios::end);

	//primary index
	Primary idx;
	strcpy(idx.ID, c.ID);
	idx.offset = dataFile.tellp();
	primaryIdx.insert(lower_bound(primaryIdx.begin(), primaryIdx.end(), idx), idx);

	//secondry index
	Secondry idxS;
	idxS.key = c.instructor;
	auto it = lower_bound(secondryIdx.begin(), secondryIdx.end(), idxS);
	int lst;

	if (it == secondryIdx.end() || it->key != idxS.key)
		lst = -1;
	else
		lst = it->labelID;

	invertedList.seekp(0, ios::end);
	idxS.labelID = invertedList.tellp() / INVERTED_SIZE;
	invertedList.write((char*)&lst, sizeof lst);
	invertedList.write(c.ID, sizeof c.ID);

	if (lst == -1)
		secondryIdx.insert(it, idxS);
	else
		it->labelID = idxS.labelID;
	writeCourse(c);
}
void readCourse(Course& c) {
	short len;

	dataFile.read((char*)&len, sizeof len);
	dataFile.getline(c.ID, sizeof c.ID, '|');
	getline(dataFile, c.name, '|');
	getline(dataFile, c.instructor, '|');
	dataFile.read((char*)&c.weeks, sizeof c.weeks);
	dataFile.get();

}
void deleteSingleCourse(bool deleteSecondry) {
	Course c;
	int off = dataFile.tellg();
	readCourse(c);

	//seek to the begining of the ID.
	dataFile.seekp(off + sizeof short(-1));
	dataFile.put('*');

	Primary tmp;
	strcpy(tmp.ID, c.ID);
	int primI = binarySearch(primaryIdx, tmp);
	primaryIdx.erase(primaryIdx.begin() + primI);
	if (deleteSecondry) {
		Secondry tmp2;
		tmp2.key = c.instructor;
		int secI = binarySearch(secondryIdx, tmp2);

		int nxt = secondryIdx[secI].labelID;
		int prev = -1;
		int cur;
		while (nxt != -1) {
			cur = nxt;
			char id[6];
			invertedList.seekg(cur * INVERTED_SIZE);
			invertedList.read((char*)&nxt, sizeof nxt);
			invertedList.read(id, sizeof id);
			if (strcmp(id, c.ID) == 0){
				if (prev == -1)
					secondryIdx[secI].labelID = nxt;
				else {
					invertedList.seekp(prev * INVERTED_SIZE);
					invertedList.write((char*)&nxt, sizeof nxt);
				}
				break;
			} 
			prev = cur;
		}
		if (secondryIdx[secI].labelID == -1)
			secondryIdx.erase(secondryIdx.begin() + secI);
	}
}

void updateCourse(const Course& c) {
	deleteSingleCourse(true);
	dataFile.seekp(0,ios::end);
	addCourse(c);
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

#pragma region PrimarySecondry


//fixed-length fields and fixed-length records
void reconstructPrimary() {
	int offset = 0;
	dataFile.seekg(offset);
	primaryIdx.clear();
	while (dataFile.peek() != EOF) {
		offset = dataFile.tellg();

		Course course;
		readCourse(course);

		if (course.ID[0] == '*') continue;

		Primary idx;
		strcpy(idx.ID, course.ID);
		idx.offset = offset;

		primaryIdx.push_back(idx);
	}
	dataFile.clear();
	sort(primaryIdx.begin(), primaryIdx.end());
}
void fetchPrimary() {
	bool uptodate = getStatus("primary.txt");
	if (!uptodate) {
		reconstructPrimary();
		return;
	}
	primaryIdx.clear();
	ifstream prim("primary.txt", ios::binary);
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
	ofstream prim("primary.txt", ios::trunc | ios::binary);
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

	//we will load the normal secondry key from the data file
	struct form {
		string key;
		char ID[6] = EMPTYID;
		bool operator < (const form& b) const {
			return key < b.key;
		}
	};
	vector<form> secon;

	dataFile.seekg(0);
	while (dataFile.peek() != EOF) {

		Course course;
		readCourse(course);

		if (course.ID[0] == '*') continue;

		form idx;
		idx.key = course.instructor;
		strcpy(idx.ID, course.ID);

		secon.push_back(idx);
	}
	dataFile.clear();

	sort(secon.begin(), secon.end());
	//we will construct the inverted list and the required secondry key index by
	//going over the groups of identical names
	invertedList.seekp(0);

	secondryIdx.clear();

	int i = 0;
	int lst;
	string last;
	int n = secon.size();
	while (i < n) {
		lst = -1;
		last = secon[i].key;

		while (i < n && secon[i].key == last) {
			invertedList.write((char*)&lst, sizeof lst);
			invertedList.write(secon[i].ID, sizeof secon[i].ID);
			lst = i++;
		}
		secondryIdx.push_back({ last,lst });
	}

}
void fetchSecondry() {
	bool uptodate = getStatus("secondry.txt");
	if (!uptodate) {
		reconstructSecondry();
		return;
	}
	secondryIdx.clear();
	ifstream sec("secondry.txt", ios::binary);
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
	ofstream sec("secondry.txt", ios::trunc | ios::binary);
	char status = 0;
	sec.put(status);
	for (Secondry cur : secondryIdx) {
		sec << cur.key << '|';
		sec.write((char*)&cur.labelID, sizeof cur.labelID);
	}
	sec.close();
}

#pragma endregion

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
			invertedList.close();
			ofstream temp;
			temp.open("data.txt", ios::trunc);
			temp.close();
			temp.open("inverted.txt", ios::trunc);
			temp.close();
			temp.open("primary.txt", ios::trunc);
			temp.close();
			temp.open("secondry.txt", ios::trunc);
			temp.close();
			atStartUtil();
			break;
		}
		case 11:
			setStatus("primary.txt", 0);
			setStatus("secondry.txt", 0);
			dataFile.close();
			invertedList.close();
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

void atStartUtil()
{
	openFile(dataFile, "data.txt");
	openFile(invertedList, "inverted.txt");
	fetchPrimary();
	fetchSecondry();
}

void addCourseUtil()
{
	

	Course c;
	cinCourse(c);
	if (c.ID[0] == 0) return;

	setStatus("primary.txt", 0);
	setStatus("secondry.txt", 0);

	addCourse(c);
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
		setStatus("primary.txt", 0);
		setStatus("secondry.txt", 0);

		dataFile.seekg(primaryIdx[i].offset);
		deleteSingleCourse(true);
	}

}

void deleteCourseInstructorUtil()
{
	Secondry idx;
	cout << "Enter Instructor: ";
	getline(cin, idx.key);
	int i = binarySearch(secondryIdx, idx);
	if (i == -1) {
		cout << "Instructor not found!\n";
	}
	else {
		setStatus("primary.txt", 0);
		setStatus("secondry.txt", 0);

		int nxt = secondryIdx[i].labelID;
		Primary idx2;
		while (nxt != -1) {
			invertedList.seekg(nxt * INVERTED_SIZE);
			invertedList.read((char*)&nxt, sizeof nxt);
			invertedList.read(idx2.ID, sizeof idx2.ID);
			int j = binarySearch(primaryIdx, idx2);
			assert(j != -1);

			Course c;
			dataFile.seekg(primaryIdx[j].offset);
			deleteSingleCourse(false);
		}
		secondryIdx.erase(secondryIdx.begin() + i);
	}
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
		dataFile.seekg(offset);
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
		int nxt = secondryIdx[i].labelID;
		Primary idx2;
		while (nxt != -1) {
			invertedList.seekg(nxt * INVERTED_SIZE);
			invertedList.read((char*)&nxt, sizeof nxt);
			invertedList.read(idx2.ID, sizeof idx2.ID);
			int j = binarySearch(primaryIdx, idx2);
			assert(j != -1);

			Course c;
			dataFile.seekg(primaryIdx[j].offset);
			readCourse(c);
			coutCourse(c);
		}
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
		setStatus("primary.txt", 0);
		setStatus("secondry.txt", 0);
		cout << "Enter new course:\n";
		Course c;
		cinCourse(c);
		dataFile.seekp(primaryIdx[i].offset);
		updateCourse(c);
	}
}

void updateCourseInstructorUtil()
{
	Secondry idx;
	cout << "Enter Instructor: ";
	getline(cin, idx.key);
	int i = binarySearch(secondryIdx, idx);
	if (i == -1) {
		cout << "Instructor not found!\n";
	}
	else {
		setStatus("primary.txt", 0);
		setStatus("secondry.txt", 0);

		int nxt = secondryIdx[i].labelID;
		Primary idx2;
		assert(nxt != -1);
		invertedList.seekg(nxt * INVERTED_SIZE);
		invertedList.read((char*)&nxt, sizeof nxt);
		invertedList.read(idx2.ID, sizeof idx2.ID);
		int j = binarySearch(primaryIdx, idx2);
		assert(j != -1);

		cout << "Enter new course for course " << idx2.ID << " :\n";
		Course c;
		cinCourse(c);
		dataFile.seekg(primaryIdx[j].offset);
		updateCourse(c);
	}
	return;

}

void atEndUtil()
{
	dataFile.close();
	flushPrimary();
	flushSecondry();
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
