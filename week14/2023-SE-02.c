#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

int find_word_start(int fd, int pos) {
    if (pos == 0) {
        return 0;
    }
    uint8_t c;
    for (int i = pos - 1; i >= 0; i--) {
        if (lseek(fd, i, SEEK_SET) < 0){
            err(9, "err lseeking to pos");
        }
        if (read(fd, &c, sizeof(c)) != sizeof(c)) {
            err(10, "err reading byte in finding word start");
        }
        if (c == 0) {
            return i;
        }
    }
    return 0;
}


int main(int argc, char* argv[]){
    if (argc != 3) {
        errx(1, "3 args needed");
    }

    const char* word = argv[1];
    int dict = open(argv[2], O_RDONLY);
    if (dict < 0) {
        err(2, "err opening dictionary");
    }

	struct stat s;
	if(fstat(dict,&s) < 0){
		err(4,"err stat");
	}
	int size = s.st_size;

    int left = 0;
    int right = size;

    char curr_word[64];

    while (left < right){
        int mid = (left+right)/2;
        int start = find_word_start(dict,mid);

        if (start < 0 || lseek(dict, start + 1, SEEK_SET) < 0) {
            errx(4, "err seeking in dict file during word search");
        }

        uint8_t c;
        int len = 0;
        while(len < 63) {
            if (read(dict, &c, sizeof(c)) != sizeof(c)){
                err(8, "err reading word");
            }
            if (c == '\n'){
                break;
            }
        curr_word[len++] = c;
        }
        curr_word[len] = '\0';

        int value = strcmp(curr_word, word);
        if (value == 0) {
            int curr_pos = lseek(dict, 0, SEEK_CUR);
            dprintf(1,"debug print: Found word at position %x\n", curr_pos);
            uint8_t byte;
            while(read(dict,&byte,sizeof(byte)) == sizeof(byte)){
                if (byte == 0) {
                    break;
                }
                if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
                    err(6, "err writing definition");
                }
            }
            byte = '\n';
            if (write(1,&byte,sizeof(byte)) != sizeof(byte)){
                err(10, "err writing endline to definition");
            }
            close(dict);
            return 0;
        }
        else if (value > 0){
            right = start;
        }
        else {
            left = mid +1;
        }

    }
    const char no_word[] = "no definition found\n";
    if (write(1,no_word,strlen(no_word)) != strlen(no_word)){
        err(7, "err writing no definition msg");
    }
    return 0;
}