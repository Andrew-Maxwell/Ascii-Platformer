#include <vector>
#include <iostream>

using namespace std;

class parent {

    public:

    virtual void func() { cout << "Base" << endl; }

};

class child : public parent {

    public:

    void func() override {
        cout << "Child" << endl;
    }
};


int main() {
    vector<parent> vec;
    vec.push_back(child());
    vec.begin() -> func();

    parent* p = &(*vec.begin());
    p -> func();

    p = new child();
    p -> func();

    vector<parent*> ptrVec;
    ptrVec.push_back(new child());
    (*ptrVec.begin()) -> func();
}
