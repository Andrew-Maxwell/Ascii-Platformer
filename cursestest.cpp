#include <ncurses.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    int count = 0;
    int in;
    initscr();
    nodelay(stdscr, TRUE);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    while (count < 100) {
        in = getch();
        cout << ++count << in << endl;
        usleep(2000000);
    }
    endwin();
}
