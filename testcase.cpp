#include <iostream>
#include <list>

using namespace std;

class entity {

    public:

    virtual bool finalize() {
        cout << "entity::finalize() should be overloaded.";
        return true;
    }
};

class collideable : virtual public entity {

    public:

    virtual bool doesCollide() {
        cout << "collideable::doescollide() should be overloaded.";
        return true;
    }
};

class particle : virtual public entity {

    int pcounter = 0;

    bool finalize() {
        cout << "particle::finalize()";
        return ++pcounter > 6;
    }

};

class physicalParticle : virtual public collideable, public particle {

    int counter = 0;

    public:

    bool doesCollide() {
        cout << "physicalparticle::doesCollide()";
        return ++counter > 3;
    }

    bool finalize() {
        cout << "physicalparticle::finalize()";
        return ++counter > 4;
    }
};

class collider {

    public:

    list<entity*> entities;

    bool finalize() {
        list<entity*>::iterator e = entities.begin();
        while (e != entities.end()) {
            if ((*e) -> finalize()) {
                delete *e;
                e = entities.erase(e);
            }
            else {
                e++;
            }
        }
        return false;
    }

};

int main() {
    collider* world = new collider();
    physicalParticle* p = new physicalParticle();
//    particle* e = new particle();
    world -> entities.push_back(p);
//    world -> entities.push_back(e);
    for (int i = 0; i < 10; i++) {
        world -> finalize();
        cout << endl;
    }
}
