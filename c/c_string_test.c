#include <stdio.h>

int main() {
    char* c = "1234567";
    for (int i = 0; i < 6; i++) {
        printf("%s\n", c + i * sizeof(char));
    }
}
