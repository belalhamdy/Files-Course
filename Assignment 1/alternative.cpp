#include <bits/stdc++.h>

using namespace std;
const int N = 10;
struct Book {
	char ISBN[6];
	string title, authorName;
	double price;
	int publishYear, nPages;
};

bool readISBN(istream& file, char* isbn);
void readString(fstream& file, string& str);

void printRecord(fstream & file);
void skipRecord(fstream& file);

int getNum(int s, int e);
Book getBookFromUser();

//reads ISBN from user and deletes book if found. Returns true if the book was found and deleted.
bool deleteBook(fstream& file, char ISBN[6]);

void saveBook(fstream& file, const Book& b);

int main() {
	fstream file("data.txt", ios::in | ios::out | ios::ate | ios::binary);
	if (!file.is_open()) {
		ofstream temp("data.txt");
		temp.close();
		file.open("data.txt", ios::in | ios::out | ios::ate | ios::binary);
	}
	cout << "Files: Assignment 1 - by Ahmed Nasr Eldardery\n";

	while (1) {
		cout << "What do you want to do?\n";
		cout <<
			"1 - Add book\n" <<
			"2 - Delete book\n" <<
			"3 - Update book\n" <<
			"4 - Print book\n" <<
			"5 - Print all books\n" <<
			"0 - Exit\n";
		int c = getNum(0, 5);
		if (c == 1) {
			Book b = getBookFromUser();
			file.seekp(0, ios::end);
			saveBook(file, b);
		}
		else if (c == 2) {
			cout << "Enter ISBN to delete:\n";
			cin.ignore(INT_MAX, '\n');
			char ISBN[6];
			readISBN(cin, ISBN);
			if (deleteBook(file, ISBN)) {
				cout << "Book was deleted successfully!\n";
			}
			else
				cout << "Couldn't find book\n";
		}
		else if (c == 3) {
			cout << "Enter ISBN to update:\n";
			cin.ignore(INT_MAX, '\n');
			char ISBN[6];
			readISBN(cin, ISBN);
			if (deleteBook(file, ISBN)) {
				Book b = getBookFromUser();
				file.seekp(0, ios::end);
				saveBook(file, b);
				cout << "Book was updated successfully!\n";
			}
			else
				cout << "Book was not found!";
		}
		else if (c == 4) {
			string name;
			cin.ignore(INT_MAX, '\n');
			cout << "Enter book name to find:\n";
			getline(cin, name);
			file.seekg(0);

			while (1) {
				while (file.peek() == '*') {
					skipRecord(file);
				}
				if (file.eof()) {
					file.clear();
					cout << "Couldn't find book\n";
					break;
				}
				
				file.seekg(5, ios::cur);
				string curName;
				readString(file, curName);
				
				int sz = curName.length();
				int movement = 5 + sz + sizeof sz;
				file.seekg(-movement, ios::cur);

				if (curName == name) {
					printRecord(file);
					file.clear();
					break;
				}
				else
					skipRecord(file);
			}
		}
		else if (c == 5) {
			file.seekg(0);
			while (!file.eof())
				printRecord(file);
			file.clear();
		}
		else if (c == 0)
			break;
	}
	file.close();
}
bool deleteBook(fstream& file, char ISBN[]) {
	char ISBNread[6];
	
	file.seekg(0);
	while (1) {
		if (!readISBN(file, ISBNread)) skipRecord(file);
		if (file.eof()) {
			file.clear();
			return 0;
		}
		if (strcmp(ISBN, ISBNread) == 0) {
			file.seekp(-5, ios::cur);
			file.write("*", 1);
			return 1;
		}
	}
}
void saveBook(fstream& file, const Book& b) {
	int sz = 5;
	file.write(b.ISBN, sz);

	sz = b.title.length();
	file.write((char*)&sz, sizeof sz);
	file.write(b.title.c_str(), sz);

	sz = b.authorName.length();
	file.write((char*)&sz, sizeof sz);
	file.write(b.authorName.c_str(), sz);

	file.write((char*)&b.price, sizeof b.price);

	file.write((char*)&b.nPages, sizeof b.nPages);

	file.write((char*)&b.publishYear, sizeof b.publishYear);

	file << '$';
}
Book getBookFromUser() {
	Book b;
	while (1) {
		cout << "Enter ISBN code (5 characters):\n";
		cin.ignore(INT_MAX, '\n');

		if (!readISBN(cin,b.ISBN))
			cout << "Can't have first character as *\n";
		else
			break;
	}
	cout << "Enter Book title:\n";
	cin.ignore(INT_MAX, '\n');
	getline(cin, b.title);
	
	cout << "Enter author name:\n";
	getline(cin, b.authorName);

	cout << "Enter price:\n";
	cin >> b.price;
	cout << "Enter publish year:\n";
	cin >> b.publishYear;
	cout << "Enter number of pages:\n";
	cin >> b.nPages;
	return b;
}

bool readISBN(istream& file, char* isbn) {
	file.read(isbn, 5);
	isbn[5] = 0;
	return isbn[0] != '*';
}

void readString(fstream& file, string& str) {
	int sz;
	char* temp;

	file.read((char*)&sz, sizeof sz);
	temp = new char[sz + 1];
	file.read(temp, sz);
	temp[sz] = 0;
	str.assign(temp);
	delete[] temp;
}

void skipRecord(fstream& file) {
	file.ignore(INT_MAX, '$');

	file.peek();
}

void printRecord(fstream& file) {
	
	Book b;
	int sz = 5;
	if (!readISBN(file, b.ISBN)) {
		skipRecord(file);
		return;
	}

	readString(file, b.title);
	readString(file, b.authorName);

	file.read((char*)&b.price, sizeof b.price);

	file.read((char*)&b.nPages, sizeof b.nPages);

	file.read((char*)&b.publishYear, sizeof b.publishYear);

	file.ignore(INT_MAX, '$');

	cout << "ISBN: " << b.ISBN << '\n';
	cout << "Title: " << b.title << '\n';
	cout << "Author name: " << b.authorName << '\n';
	cout << "Price: " << b.price << '\n';
	cout << "Publish year: " << b.publishYear << '\n';
	cout << "Number of pages: " << b.nPages << '\n';
	cout << "----------------\n";

	file.peek();
}

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
