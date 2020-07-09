#ifndef INPUT_HPP
#define INPUT_HPP
#include "meta.hpp"
#include <raylib.h>

using namespace std;

//A single key or gamepad axis

int myGetKeyPressed();

int getAnyGamepadInput();

int selectGamepad();

struct input {
    bool isAxis;  //True if it's a stick/trigger; false if it's a button
    bool positive;  //True if it's the positive half of the axis
    int id = 0;     //Key/button/axis number
    float value = 0, lastValue = 0;
    bool firstCallPressed, firstCallReleased;

    input();

    input(bool newIsAxis, bool newPositive, int newID);

    bool isDefined();

    string name(int device);

    void update(int device);

    bool isDown();

    bool isPressed();

    bool isPressedOnce();   //Only returns true once per tick

    bool isReleased();

    bool isReleasedOnce();  //Only returns true once per tick

    bool isUp();

};

struct inputMap {
    input error, up, down, left, right, jump, interact, inventory, nextWeapon, previousWeapon, explode, lastCode, nextCode, previousCode, aimUp, aimDown, aimLeft, aimRight, fire;
    input code[10];

    bool keyboard;
    bool useMouseAim;
    int device;

    inputMap(int newDevice = -1);

    void update();

    input& operator[](int index);

    string name(int index);

    bool change(int index);

    int count();

    Vector2 getAim();
};

string keyName(int key);

#endif //INPUT_HPP
