#include <iostream>
#include <vector>

using namespace std;

struct little {
    int data;
};

struct big {
    char c;
    little l;
};

class bigger {

    vector<string> strs;
    big b;

    public:

    void init(int x, char c) {
        b.c = c;
        b.l.data = x;
    }

    bigger(int x, char c) {
        strs.push_back("hello");
        strs.push_back("world");
        init(x, c);
    }

    void print() {
        cout << b.c << " " << b.l.data << endl;
    }

};

int main() {
    bigger b(100, 'a');
    b.print();
}
