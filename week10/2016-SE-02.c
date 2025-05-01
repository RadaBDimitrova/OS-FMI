#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "3 args needed");
    }

    int f1 = open(argv[1], O_RDONLY);
    if (f1 < 0) {
        err(2, "can't open f1");
    }

    int f2 = open(argv[2], O_RDONLY);
    if (f2 < 0) {
        err(3, "can't open f2");
    }

    int f3 = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (f3 < 0) {
        err(4, "can't open f3");
    }

    struct stat st;
    if (fstat(f1, &st) < 0) {
        err(5, "fstat fail");
    }
    if (st.st_size % (2 * sizeof(uint32_t)) != 0) {
        errx(6, "f1 size doesn't have correct size (/8)");
    }

    uint32_t xi, yi;
    // uint32_t xi, yi; // pair of numbers
    // could be a struct, for this task it's ok to use two variables
    // struct pair {
    //     uint32_t x;
    //     uint32_t y;
    // };
    // struct pair p;
    // while (read(f1, &p, sizeof(p)) == sizeof(p)) {
    //     ...
    // }
    int xnum, ynum;
    while ( (xnum = read(f1, &xi, sizeof(xi))) == sizeof(xi) && (ynum = read(f1, &yi, sizeof(yi))) == sizeof(yi)) {
        
        if (lseek(f2, xi * sizeof(uint32_t), SEEK_SET) < 0) {
            err(7, "lseek fail");
        }

        for (uint32_t i = 0; i < yi; i++) {
            uint32_t num;
            if (read(f2, &num, sizeof(num)) != sizeof(num)) {
                err(8, "failed read from f2");
            }
            if (write(f3, &num, sizeof(num)) != sizeof(num)) {
                err(9, "failed write to f3");
            }
        }
    }

    if (xnum < 0 || ynum < 0) {
        err(10, "failed read of pair");
    }

    close(f1);
    close(f2);
    close(f3);
    return 0;
}
