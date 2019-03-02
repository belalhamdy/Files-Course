#include <iostream>
#include <cstring>
#include <fstream>
#define L cout << file.tellg() << " " << file.tellp() << endl;
using namespace std;

const short ISBNsiz = 6, stringSiz = 20;
const char delim = '%', del = '*';

struct Book
{
    char* ISBN;
    short titlesiz, authorsiz;
    char* title;
    char* author;
    ~Book()
    {
        delete ISBN;
        delete title;
        delete author;
    }
};

void addBook(fstream& file);
bool printTitle(fstream& file,string req);
bool deleteBook (fstream& file);
bool updateBook (fstream& file);
int getNext(fstream& file);

ostream& operator << (ostream& out, const Book& b);

int main()
{
    fstream file;
    ofstream tmpfile;
    string filename;
    cout << "Enter filename : ";
    getline(cin,filename);
    tmpfile.open(filename);
    tmpfile.close();
    file.open(filename, ios :: binary | ios :: in | ios :: out | ios :: ate);

    if (!file.is_open())
    {
        cout << "Error in opening file\n";
        return 0;
    }

    int choose;
    do
    {
        cout << "0- Save and Close\n1- Add book\n2- Delete book\n3- Update book\n4- Print book(by title)\n5- Print all\n";
        cin >> choose;
        switch(choose)
        {
        case 1:
            addBook(file);
            break;
        case 2 :
            cout << (deleteBook(file)? "Book is deleted successfully\n" : "Book is not found\n") ;
            break;
        case 3 :
            cout << (updateBook(file)? "Book is updated successfully\n" : "Book is not found\n") ;
            break;
        case 4 :
        {
            string req;
            cout << "Enter the title : ";
            cin.ignore();
            getline(cin,req);
            if(!printTitle(file,req)) cout << "Book is not found\n";
            break;
        }
        case 5 :

            printTitle(file,"");
            break;
        default :
            file.close();
            return 0;
        }
    }
    while (choose);

    return 0;
}

void addBook(fstream& file)
{
    Book book;
    string tmp;

    cout << "Enter ISBN (of 5 characters) : ";
    cin.ignore(INT_MAX,'\n');
    book.ISBN = new char [ISBNsiz];
    getline(cin,tmp);
    for (int i = 0 ; i<ISBNsiz-1 ; ++i) book.ISBN[i] = tmp[i];
    book.ISBN[ISBNsiz-1] = '\0';
    tmp.clear();

    cout << "Enter title of the book : ";
    //cin.ignore(INT_MAX,'\n');
    getline(cin,tmp);
    book.titlesiz = tmp.size()+1;
    book.title = new char [book.titlesiz];
    strcpy(book.title,tmp.c_str());
    tmp.clear();

    cout << "Enter author of the book : ";
    getline(cin,tmp);
    book.authorsiz = tmp.size()+1;
    book.author = new char [book.authorsiz];
    strcpy(book.author,tmp.c_str());

    file.seekp(0,ios::end);
    file.write((char*)&ISBNsiz,sizeof(ISBNsiz));
    file.write(book.ISBN, ISBNsiz);
    file.write((char*) &book.titlesiz,sizeof(book.titlesiz));
    file.write (book.title,book.titlesiz);
    file.write((char*) &book.authorsiz, sizeof(book.authorsiz));
    file.write (book.author,book.authorsiz);

    file.write(&delim,1);
}

int getNext(fstream& file, Book& b)  // reads the current book and returns the idx of the next
{

    short isbns = 1234 ;
    char d;
    if (file.peek() == EOF)
    {
        file.clear();
        return -1;
    }

    file.read((char*)&isbns, sizeof(isbns));
    b.ISBN = new char [ISBNsiz];
    file.read (b.ISBN,isbns);
    /**/if (file.fail()) return -1;
    file.read ((char*)&b.titlesiz,sizeof (b.titlesiz));
    b.title = new char [b.titlesiz];
    file.read (b.title,b.titlesiz);
    file.read ((char*)& b.authorsiz,sizeof (b.authorsiz));
    b.author = new char [b.authorsiz];
    file.read (b.author,b.authorsiz);
    file.read (&d, 1);

    if (file.fail()) return -1;

    return file.tellp();
}
bool deleteBook (fstream& file)
{
    int curr = 0,nxt = 0;
    Book curbook;

    string req;
    cout << "Enter ISBN : ";

    cin >> req;
    file.seekp(0);
     if (file.peek() == EOF || req.size() != ISBNsiz-1)
    {
        file.clear();
        return 0;
    }

    while (curr != -1)
    {
        nxt = getNext(file,curbook);
        if (curbook.ISBN == req)
        {
            file.seekp(curr,ios::beg);
            cout << sizeof (ISBNsiz) << endl;
            file.seekp(sizeof(ISBNsiz), ios :: cur);
            file.write(&del, 1);
            file.seekp(0);
            return 1;
        }

        curr = nxt;
    }
    return 0;
}
bool updateBook(fstream& file)
{
    if (deleteBook(file))
    {
        cout << "Enter the data of new book  \n\n";
        addBook(file);
        return 1;
    }

    return 0;
}
bool printTitle(fstream& file, string req)
{
    bool ret = 0;
    Book cur;
    file.seekp(0);
    int f = file.peek();
    if (file.peek() == EOF)
    {
        file.clear();
        return 0;
    }
    int n = getNext(file,cur);
    do
    {
        if (cur.ISBN[0] != del && (cur.title == req || !req.size()))
        {
            ret = 1;
            cout << cur << "\n\n";
        }
        n = getNext(file,cur);
    }
    while(n != -1);
    return ret;
}

ostream& operator << (ostream& out, const Book& b)
{
    out << "ISBN : " << b.ISBN << "\nTitle : " << b.title << "\nAuthor : " << b.author;
    return out;
}
