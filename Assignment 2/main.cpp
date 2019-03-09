#include <bits/stdc++.h>

using namespace std;
#define SZB sizeof Book

struct Book {
	char ISBN[6] = {};
	char title[100] = {}, author[100] = {};
	double price = 0;
	int publishYear = 0, nPages = 0;
};

const int RECORD_SIZE =
SZB::ISBN +
SZB::title + SZB::author +
SZB::price +
SZB::publishYear + SZB::nPages;

const int OPEN_MODE = ios::in | ios::out | ios::ate | ios::binary;

const char* FILE_NAME = "data.txt";
const char DELETE_MARK = '*';

void AddBookUtil(fstream& file);
void DeleteBookUtil(fstream& file);
void UpdateBookUtil(fstream& file);
void PrintBookUtil(fstream& file);
void PrintAllUtil(fstream& file);
void CompactFileUtil(fstream& file);

int getNum(int s, int e);
int main() {
	fstream file(FILE_NAME, OPEN_MODE);
	if (!file.is_open()) {
		ofstream temp(FILE_NAME);
		temp.close();
		file.open(FILE_NAME, OPEN_MODE);
	}

	//initializes deleted list
	file.seekg(0, ios::end);
	if (file.tellg() == 0) {
		file.seekp(0);
		short head = -1;
		file.write((char*)&head, sizeof head);
	}
	cout << "Files: Assignment 2 - by Ahmed Dardery - Belal Hamdy\n";

	while (1) {
		cout << "What do you want to do?\n";
		cout <<
			"1 - Add book\n" <<
			"2 - Delete book\n" <<
			"3 - Update book\n" <<
			"4 - Print book\n" <<
			"5 - Print all books\n" <<
			"6 - Compact File\n" <<
			"0 - Exit\n";
		int c = getNum(0, 6);
		cin.ignore(INT_MAX, '\n');

		switch (c) {
		case 1:
			AddBookUtil(file);
			break;
		case 2:
			DeleteBookUtil(file);
			break;
		case 3:
			UpdateBookUtil(file);
			break;
		case 4:
			PrintBookUtil(file);
			break;
		case 5:
			PrintAllUtil(file);
			break;
		case 6:
			CompactFileUtil(file);
			break;
		case 0:
			file.close();
			return 0;
		}
	}
}


Book getBookFromUser() {
	Book b;
	cout << "Enter ISBN code (5 characters):\n";
	cin.getline(b.ISBN, sizeof b.ISBN);

	cout << "Enter Book title:\n";
	cin.getline(b.title, sizeof b.title);

	cout << "Enter author name:\n";
	cin.getline(b.author, sizeof b.author);

	cout << "Enter price:\n";
	cin >> b.price;

	cout << "Enter publish year:\n";
	cin >> b.publishYear;

	cout << "Enter number of pages:\n";
	cin >> b.nPages;

	return b;
}
//Prints content of book to stdout
void printBook(const Book& b) {
	cout << "ISBN: " << b.ISBN << '\n';
	cout << "Title: " << b.title << '\n';
	cout << "Author name: " << b.author << '\n';
	cout << "Price: " << b.price << '\n';
	cout << "Publish year: " << b.publishYear << '\n';
	cout << "Number of pages: " << b.nPages << '\n';
	cout << "----------------\n";
}

void writeRecord(fstream & file, const Book & b) {
	file.write(b.ISBN, sizeof b.ISBN);
	file.write(b.title, sizeof b.title);
	file.write(b.author, sizeof b.author);
	file.write((char*)&b.price, sizeof b.price);
	file.write((char*)&b.publishYear, sizeof b.publishYear);
	file.write((char*)&b.nPages, sizeof b.nPages);
}
void readRecord(fstream & file, Book & b) {
	file.read(b.ISBN, sizeof b.ISBN);
	file.read(b.title, sizeof b.title);
	file.read(b.author, sizeof b.author);
	file.read((char*)&b.price, sizeof b.price);
	file.read((char*)&b.publishYear, sizeof b.publishYear);
	file.read((char*)&b.nPages, sizeof b.nPages);
}

void deleteRecord(fstream& file) {
	short pos = file.tellp() / RECORD_SIZE;
	short top;

	//read current top and put it in the new position rem data
	//update the current top to the new postion
	//tl;dr: adds pos to the stack
	file.seekg(0);
	file.read((char*)&top, sizeof top);
	file.seekp(0);
	file.write((char*)&pos, sizeof pos);

	file.seekp(pos * RECORD_SIZE + sizeof(short));
	file.put(DELETE_MARK);
	file.write((char*)&top, sizeof top);

}

//Seeks to the top of the deleted stack, or to the end of the file if
//it is empty
void seekAddRecord(fstream & file) {
	file.seekg(0);
	short top, pos;
	//get the position of the last deleted record
	file.read((char*)&top, sizeof top);

	//if no records are deleted, append new record to the end
	if (top == -1) {
		file.seekp(0, ios::end);
		return;
	}
	//read the rem data in the top (the next one down the stack) and put it instead
	//as the new top

	//tl;dr: remove the top from the stack
	pos = top * RECORD_SIZE + sizeof(short);
	file.seekg(pos);
	assert(file.get() == DELETE_MARK);
	file.read((char*)&top, sizeof top);

	file.seekg(0);
	file.write((char*)&top, sizeof top);

	file.seekp(pos);
}

//Searches for the file by isbn, returns whether it was found or not
bool seekFindRecord(fstream &file, char isbn[]) {
	file.seekg(sizeof(short));
	char tmp[SZB::ISBN];
	while (file.peek() != EOF) {
		file.read(tmp, sizeof tmp);
		if (strcmp(tmp, isbn) == 0) {
			//seek back the ISBN size so that g is at the start of the found match
			file.seekg(-int(SZB::ISBN), ios::cur);
			return 1;
		}
		else {
			//seek forward the rest of the current record
			file.seekg(RECORD_SIZE - SZB::ISBN, ios::cur);
		}
	}
	file.clear();
	return 0;
}

void AddBookUtil(fstream& file) {
	Book b = getBookFromUser();
	seekAddRecord(file);
	writeRecord(file, b);
}

void DeleteBookUtil(fstream& file) {
	char ISBN[6];
	cout << "Enter ISBN code (5 characters):\n";
	cin.getline(ISBN, sizeof ISBN);

	if (!seekFindRecord(file, ISBN)) {
		cout << "Book not found!\n";
	}
	else {
		deleteRecord(file);
	}
}

void UpdateBookUtil(fstream& file) {
	char ISBN[6];
	cout << "Enter ISBN code (5 characters):\n";
	cin.getline(ISBN, sizeof ISBN);

	if (!seekFindRecord(file, ISBN)) {
		cout << "Book not found!\n";
	}
	else {
		Book b = getBookFromUser();
		writeRecord(file, b);
	}
}

void PrintBookUtil(fstream& file) {
	char ISBN[6];
	cout << "Enter ISBN code (5 characters):\n";
	cin.getline(ISBN, sizeof ISBN);

	if (!seekFindRecord(file, ISBN)) {
		cout << "Book not found!\n";
	}
	else {
		Book b;
		readRecord(file, b);
		printBook(b);
	}
}

void PrintAllUtil(fstream& file) {
	file.seekg(sizeof(short));
	//cerr << file.tellg();
	Book b;
	while (file.peek() != EOF) {
		readRecord(file, b);
		if (b.ISBN[0] != DELETE_MARK)
			printBook(b);
	}
	file.clear();
}

void CompactFileUtil(fstream & file){
	file.seekg(sizeof(short));
	vector<Book> books;
	Book b;
	while (!file.eof()) {
		readRecord(file, b);
		if (b.ISBN[0] != DELETE_MARK)
			books.push_back(b);
			
		file.peek();
	}
	file.close();
	file.open(FILE_NAME, ios::in | ios::out | ios::trunc | ios::binary);
	short avail = -1;
	file.write((char*)&avail, sizeof avail);
	for (Book b : books) {
		writeRecord(file, b);
	}
}

//gets number from user, forces it to be between s and e
int getNum(int s, int e)
{

	int n;
	while (true)
	{
		cin >> n;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "Invalid input.\n";
		}
		else if (n > e || n < s)
			cout << "Invalid input. only (" << s << " , " << e << ") allowed only\n";
		else
			return n;
	}
}
