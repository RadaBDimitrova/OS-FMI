#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <sys/stat.h>

struct complect {
    uint16_t offset;
    uint8_t length;
    uint8_t padding;
};

int main(int argc, char* argv[]) {
    if (argc != 5) {
        errx(4, "4 args needed");
    }

    int fdidx1 = open(argv[2], O_RDONLY);
    if (fdidx1 < 0) err(1, "err opening f1.idx");

    int fddat1 = open(argv[1], O_RDONLY);
    if (fddat1 < 0) err(1, "err opening f1.dat");

    int fdidx2 = open(argv[4], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fdidx2 < 0) err(1, "err creating f2.idx");

    int fddat2 = open(argv[3], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fddat2 < 0) err(1, "err creating f2.dat");

    struct stat s;
    if (fstat(fdidx1, &s) < 0) err(2, "can't stat");

    if (s.st_size % sizeof(struct complect) != 0) {
        errx(8, "invalid .idx file format");
    }

    int entries = s.st_size / sizeof(struct complect);
    struct complect c;
    struct complect new_entry;
    uint16_t new_offset = 0;
    uint8_t buffer[256]; // максимум 255 байта

    for (int i = 0; i < entries; i++) {
        if (read(fdidx1, &c, sizeof(c)) != sizeof(c)) {
            err(3, "err reading index");
        }

        if (lseek(fddat1, c.offset, SEEK_SET) == -1) {
            err(4, "err seeking in dat");
        }

        if (c.length == 0 || c.length > sizeof(buffer)) {
            continue;
        }

        if (read(fddat1, buffer, c.length) != c.length) {
            err(5, "err reading bytes");
        }

        if (buffer[0] < 'A' || buffer[0] > 'Z') { //byte < 0x41 || byte > 0x5A
            continue;
        }

        if (write(fddat2, buffer, c.length) != c.length) {
            err(6, "err writing to f2.dat");
        }

        new_entry.offset = new_offset;
        new_entry.length = c.length;
        new_entry.padding = 0;

        if (write(fdidx2, &new_entry, sizeof(new_entry)) != sizeof(new_entry)) {
            err(7, "err writing to f2.idx");
        }

        new_offset += c.length;
    }

    close(fdidx1);
    close(fddat1);
    close(fdidx2);
    close(fddat2);

    return 0;
}