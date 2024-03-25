#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    unsigned char** data;
    unsigned int length;
} List;

List readDir(const char* dir, int system) {
    char* command = "";

    if (system == 0) {
        command = "dir /b";
    } else if (system == 1) {
        command = "ls";
    } else {
        printf("\x1b[31mUnknown system type\x1b[0m");
        List l;
        return l;
    }

    printf(command);

    List l;
    return l;
}

int main(int argv, const char* args) {
    int system = 0; // 0 - MS-DOS, 1 - Unix

    List l = readDir("", system);
}