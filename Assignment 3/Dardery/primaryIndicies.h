#ifndef _PRIMARY_INDICIES
#define _PRIMARY_INDICIES

#include <cstring>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Course.h"

using namespace std;
#define EMPTYID { ' ',' ',' ',' ',' ',' ' }

class PrimaryIndicies {
	
public:
	const char* FILE_NAME = "primary.txt";

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


	bool getStatus() {
		return ::getStatus(FILE_NAME);
	}
	void setStatus(bool a) {
		::setStatus(FILE_NAME, a);
	}

	void insertIndexFor(const Course& c, int offset) {
		setStatus(0);
		Primary idx;
		strcpy(idx.ID, c.ID);
		idx.offset = offset;
		primaryIdx.insert(lower_bound(primaryIdx.begin(), primaryIdx.end(), idx), idx);
	}
	void removeIndexFor(const Course& c) {
		setStatus(0);
		int primI = findIndexFor(c.ID);
		primaryIdx.erase(primaryIdx.begin() + primI);
	}
	int findIndexFor(const char id[6]) {
		Primary tmp;
		strcpy(tmp.ID, id);
		return binarySearch(primaryIdx, tmp);
	}
	Primary& operator[](int i) {
		return primaryIdx[i];
	}

	//fixed-length fields and fixed-length records
	void reconstructPrimary(fstream& dataFile) {
		int offset = 0;
		primaryIdx.clear();
		dataFile.seekg(0);
		while (dataFile.peek() != EOF) {
			offset = dataFile.tellg();

			Course course;
			readCourse(-1, dataFile, course);

			if (course.ID[0] == DELETE_MARK) continue;

			Primary idx;
			strcpy(idx.ID, course.ID);
			idx.offset = offset;

			primaryIdx.push_back(idx);
		}
		dataFile.clear();
		sort(primaryIdx.begin(), primaryIdx.end());
	}
	void fetchPrimary(fstream& dataFile) {
		bool uptodate = getStatus();
		if (!uptodate) {
			reconstructPrimary(dataFile);
			return;
		}
		primaryIdx.clear();
		ifstream prim(FILE_NAME, ios::binary);
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
		ofstream prim(FILE_NAME, ios::trunc | ios::binary);
		char status = 0;
		prim.put(status);
		for (Primary cur : primaryIdx) {
			prim.write(cur.ID, sizeof cur.ID);
			prim.write((char*)&cur.offset, sizeof cur.offset);
		}
		prim.close();
	}
private:
	vector<Primary> primaryIdx;
};


#endif