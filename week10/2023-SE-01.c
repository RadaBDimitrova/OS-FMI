#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>

uint8_t check(const uint8_t* buff,uint8_t size);
uint8_t check(const uint8_t* buff,uint8_t size){
	uint8_t res=0;	
	for(uint8_t i = 0; i < size; i++){
		res^=buff[i];		
	}
	return res;
}


int main(int argc,char* argv[]){
	if(argc != 3){
		errx(1,"2 args needed");
	}

	int ifd = open(argv[1],O_RDONLY);
	if(ifd < 0){
		err(2,"err opening input file");
	}

	int ofd = open(argv[2],O_CREAT|O_TRUNC|O_RDWR,S_IRUSR|S_IWUSR);
	if(ofd < 0){
		err(3,"err creating output file");
	}

	uint8_t buff[256];
	uint8_t byte;
	uint8_t N  = 0;
	uint8_t checksum = 0;
	
    int readbyte;
  while((readbyte = read(ifd, &byte, sizeof(byte))) == sizeof(byte)){
	
    if(byte == 0x55){
			buff[0] = byte;
		
      if(read(ifd, &N, sizeof(N)) != sizeof(N)){
				err(4,"err reading N");
	    }
			
      buff[1]=N;
      if (N < 3) {
        errx(5, "N should be > 3");
      }

      for(int i = 2; i < N-1; i++){
			
        if((readbyte = read(ifd, &buff[i], sizeof(buff[i]))) != sizeof(buff[i])){
            if (readbyte == 0){
                err(6, "reached end before end of N");
            }
				err(6,"err reading buff");
			}	
	  }

		if(read(ifd, &checksum, sizeof(checksum)) != sizeof(checksum)){
			err(7,"err reading checksum");
		}
			
		if(checksum == check(buff,N-1)){
			buff[N-1] = checksum;
				
            if(write(ofd, &buff, N) != N){
				err(8,"err writing valid message");
			}
		}
	}

	}
	close(ifd);
	close(ofd);
	return 0;
}