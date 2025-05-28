#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc,char* argv[]){
	if(argc!=3){
		errx(1,"2 filenames needed as args");
	}

	int infd = open(argv[1],O_RDONLY);
	if(infd < 0 ){
		err(2, "err opening source");
	}

	struct stat s;
	if( fstat(infd,&s) < 0){
		err(4,"err stat");
	}
	int size = s.st_size;

	int ofd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,S_IWUSR|S_IRUSR);
	if(ofd <0){
		err(3,"err creating output file");
	}
	uint8_t byte;
	for(int i = 0; i < size; i++){
		if(read(infd,&byte,sizeof(byte)) != sizeof(byte)){
			err(5,"err reading byte");
		}
		uint16_t toSend = 0;
		for(int j = 0; j < 8; j++){
			uint8_t mask = 1 << (7 - j); // mask is 10000000, 01000000, ..., 00000001
			// mask & byte checks if the j-th bit in byte is set
			if(mask & byte){
				toSend |= 1 << (15-j*2); // set the 15-j*2-th bit in toSend, so the 2 bits are set to 10

			}
			else {
				toSend |= 1 <<(14-j*2); // set the 14-j*2-th bit in toSend, so the 2 bits are set to 01
			}
		}

		//0xFF is 255 so 1111111 in binary
		uint8_t high = (toSend >> 8) & 0xFF; //most significant byte
		uint8_t low  = toSend & 0xFF; //least significant byte

		if (write(ofd, &high, sizeof(high)) != sizeof(high) ||
		    write(ofd, &low, sizeof(low)) != sizeof(low)) {
			err(6, "err writing output file");
		}
	}
	close(infd);
	close(ofd);
}