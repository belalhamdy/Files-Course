#ifndef _SECONDRY_INDICIES
#define _SECONDRY_INDICIES

#include <fstream>
#include <string>
#include <algorithm>
#include "Course.h"
using namespace std;


class SecondryIndicies {
	fstream invertedList;
	const char* INVERTED_NAME = "inverted.txt";


	const int INVERTED_SIZE = sizeof(-1) + sizeof Course::ID;

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
	vector<Secondry> secondryIdx;

public:
	SecondryIndicies() {
		invertedList.open(INVERTED_NAME, ios::in | ios::out | ios::binary);
		if (!invertedList.is_open()) {
			invertedList.open(INVERTED_NAME, ios::out | ios::binary);
			invertedList.close();
			invertedList.open(INVERTED_NAME, ios::in | ios::out | ios::binary);
			if (!invertedList.is_open()) exit(0);
		}
	}
	const char* FILE_NAME = "secondry.txt";

	bool getStatus() {
		return ::getStatus(FILE_NAME);
	}
	void setStatus(bool a) {
		::setStatus(FILE_NAME, a);
	}

	void readInvertedItem(int cur, int& nxt, char id[6]) {
		invertedList.seekg(cur * INVERTED_SIZE);
		invertedList.read((char*)&nxt, sizeof nxt);
		invertedList.read(id, 6 * sizeof id[0]);
	}
	void insertIndexFor(const Course&c) {
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

	}
	void removeIndexFor(const Course& c) {
		int secI = findIndexFor(c.instructor);

		int nxt = secondryIdx[secI].labelID;
		int prev = -1;
		int cur;
		while (nxt != -1) {
			cur = nxt;
			char id[6];

			readInvertedItem(cur, nxt, id);

			if (strcmp(id, c.ID) == 0) {
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
	void removeIndexAt(int i) {
		secondryIdx.erase(secondryIdx.begin() + i);
	}
	int findIndexFor(const string& instructor) {
		Secondry temp;
		temp.key = instructor;
		return binarySearch(secondryIdx, temp);

	}

	Secondry& operator[](int i) {
		return secondryIdx[i];
	}

	//fixed-length and fixed-length records
	void reconstructSecondry(fstream& dataFile) {

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
			readCourse(-1, dataFile, course);

			if (course.ID[0] == DELETE_MARK) continue;

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
	void fetchSecondry(fstream& dataFile) {
		bool uptodate = getStatus();
		if (!uptodate) {
			reconstructSecondry(dataFile);
			return;
		}
		secondryIdx.clear();
		ifstream sec(FILE_NAME, ios::binary);
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
		ofstream sec(FILE_NAME, ios::trunc | ios::binary);
		char status = 0;
		sec.put(status);
		for (Secondry cur : secondryIdx) {
			sec << cur.key << '|';
			sec.write((char*)&cur.labelID, sizeof cur.labelID);
		}
		sec.close();
	}

};

#endif