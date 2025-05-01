#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>

struct patch {
    uint16_t offset;
    uint8_t byte1;
    uint8_t byte2;
} patch_t;

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

    int pfile = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (pfile < 0) {
        err(4, "can't create and open patch file");
    }

    struct stat st1, st2;
    if (fstat(f1, &st1) < 0 || fstat(f2, &st2) < 0) {
        err(5, "fstat fail");
    }
    if (st1.st_size != st2.st_size) {
        errx(6, "differenet file sizes");
    }

    uint8_t byte1, byte2;
    uint16_t offset = 0;
    int read1, read2;
    while ( (read1 = read(f1, &byte1, sizeof(byte1))) == sizeof(byte1) && (read2 = read(f2, &byte2, sizeof(byte2))) == sizeof(byte2)) {
        if (byte1 != byte2) {
            patch_t p;
            p.offset = offset;
            p.byte1 = byte1;
            p.byte2 = byte2;
            if (write(pfile, &p, sizeof(p)) != sizeof(p)) {
                err(7, "failed write to patch file");
            }
        }
        offset++;
    }

    if (read1 < 0 || read2 < 0) {
        err(8, "failed read of files to patch");
    }

    close(f1);
    close(f2);
    close(pfile);
    return 0;
}
