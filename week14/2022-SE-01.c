#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>

struct complect {
        uint16_t type;
        uint16_t reserved[3];
        uint32_t offset1;
        uint32_t offset2;
};

struct headerd {
        uint32_t magic;
        uint32_t count;
};

struct headerc {
        uint32_t magic1;
        uint16_t magic2;
        uint16_t reserved;
        uint64_t count;
};

void assertC(struct headerc h, uint64_t size);
void assertD(struct headerd h, uint32_t size);
void assertComplect(struct complect c);

void assertComplect(struct complect c){
        for(int i = 0; i < 3; i++){
                if(c.reserved[i] != 0){
                        errx(17,"reserved values in complect can only be 0");
                }
        }
}

void assertD(struct headerd h, uint32_t size){
        if(h.magic != 0x21796F4A){
                errx(11,"specification is not correct");
        }
        if(h.count != size){
                errx(13,"count in header and size of file do not correspond");
        }
}


void assertC(struct headerc h, uint64_t size){
        if(h.magic1 != 0xAFBC7A37){
                //uint32_t m = 0xAFBC7A37;
                errx(11,"specification1 is not correct");
        }

        if(h.magic2 != 0x1C27){
                errx(11,"specification2 is not correct");
        }

        if(h.count != size){
                errx(13,"count in header and size of file do not correspond");
        }
}

int main(int argc, char* argv[]){
        if(argc!=3){
                errx(1,"3 args needed");
        }

        int datafd = open(argv[1],O_RDWR);
        if(datafd<0){
                err(2,"err opening list");
        }

        struct complect c;
        struct headerd l;
        struct stat sl;

        if(fstat(datafd,&sl) < 0){
                err(3,"err stat");
        }

        uint32_t sizel = sl.st_size - sizeof(l);
        if (sizel % sizeof(uint64_t) != 0){
                errx(9,"invalid data format");
        }

        if(read(datafd,&l,sizeof(l)) != sizeof(l)){
                err(10,"incomplete read");
        }
        sizel/=sizeof(uint64_t);
        assertD(l,sizel);

        int compfd = open(argv[2],O_RDONLY);
        if(compfd < 0){
                err(2,"err opening comparator file");
        }

        struct headerc d;
        struct stat sd;
        if(fstat(compfd,&sd) < 0){
                err(3,"err stat");
        }
        uint64_t sized = sd.st_size - sizeof(d);
        if (sized % sizeof(c) != 0){
                errx(9,"invalid comparator file format");
        }

        if(read(compfd,&d,sizeof(d)) != sizeof(d)){
                err(10,"incomplete read");
        }
        sized/=sizeof(c);
        assertC(d,sized);

        uint64_t num1;
        uint64_t num2;
        while(read(compfd,&c,sizeof(c)) == sizeof(c)){

                if (c.type != 0 && c.type != 1){
                continue;
                }
                assertComplect(c);
                if (c.offset1 >= sizel || c.offset2 >= sizel) {
                    errx(12, "offset bigger than file size");
                }
                if(lseek(datafd,c.offset1*sizeof(uint64_t) + sizeof(l),SEEK_SET) < 0){
                        err(13,"err seeking in data");
                }
                if(read(datafd,&num1,sizeof(num1)) != sizeof(num1)){
                        err(14,"err reading num1 from data");
                }

                if(lseek(datafd,c.offset2*sizeof(uint64_t) + sizeof(l),SEEK_SET) < 0){
                        err(13,"err seeking in data");
                }
                if(read(datafd,&num2,sizeof(num2)) != sizeof(num2)){
                        err(14,"err reading num2 from data");
                }

                //dprintf(1,"type:%d,num1:%ld,num2:%ld\n",c.type, num1, num2);
                if((c.type == 0 && num1 > num2) || (c.type == 1 && num1 < num2)){

                        if(lseek(datafd,c.offset1*sizeof(uint64_t) + sizeof(l),SEEK_SET) < 0){
                                err(13,"err seeking in data");
                        }
                        if(write(datafd,&num2,sizeof(num2)) != sizeof(num2)){
                                err(15,"err swapping num2 in data");
                        }

                        if(lseek(datafd,c.offset2*sizeof(uint64_t) + sizeof(l),SEEK_SET) < 0){
                                err(13,"err seeking in data");
                        }
                        if(write(datafd,&num1,sizeof(num1)) != sizeof(num1)){
                                err(15,"err swapping num1 in data");
                        }
                }

        }

}