#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>

const int MAX_TRIPLES = 8;

void waitProcess(void){
	int status;
	if(wait(&status) < 0){
		err(9,"can't wait");
	}
	if(!WIFEXITED(status)){
		err(8,"didn't exit");
	}
	if(WEXITSTATUS(status) != 0){
		err(7,"didn't exit with 0");
	}
}

struct complect { // __attribute__((packed)) - just an fyi - this can be used to do packed structs in C
	char filename[8];
	uint32_t offset;
	uint32_t length;
} complect_t;

int main(int argc, char* argv[]){

	if (argc != 2) {
		errx(1, "1 arg needed");
	}

	int fd = open(argv[1],O_RDONLY);
	if (fd < 0){
		err(2,"err open for reading");
	}

	struct stat s;
	if (fstat(fd,&s) < 0){
		err(3,"err stat");
	}

	complect_t c;
	if (s.st_size % sizeof(c) != 0){
		errx(4,"wrong format");
	}
	
	int pipefd[2];
	if (pipe(pipefd) < 0){
		err(6,"err pipe");
	}
	int size = s.st_size/sizeof(c);
    if (size > MAX_TRIPLES){
        errx(1,"too many triples");
    }

    uint16_t res = 0;
	int readbytes;
	for (int i = 0; i < size; i++){
        if ((readbytes = read(fd,&c,sizeof(c))) != sizeof(c)){
			err(5,"err reading complect");
		}

		pid_t pid = fork();
		if (pid < 0){
			err(7, "err fork");
		}

		if (pid == 0){
			close(pipefd[0]);
			
			int filefd = open(c.filename,O_RDONLY);
			if (filefd < 0){
				err(8,"err open filefd");
			}	
			
			struct stat f;
			if (fstat(filefd,&f) < 0){
				err(10,"err file stat");
			}
			
			if (f.st_size/sizeof(uint16_t) < c.offset || f.st_size/sizeof(uint16_t) < c.offset + c.length){
				errx(11,"incorrect format");
			}
			
			if (lseek(filefd,c.offset*sizeof(uint16_t),SEEK_SET) < 0){
				err(9,"err lseek");
			}

			uint16_t result = 0;
			uint16_t toxor;
			for(int j = 0; j < c.length; j++){
				if ((readbytes = read(filefd,&toxor,sizeof(uint16_t))) != sizeof(uint16_t)){
					err(12,"err reading from filefd");
				}
				result^=toxor;
			}
			
			close(filefd);
			
			if (write(pipefd[1],&result,sizeof(result)) < sizeof(result)){
				err(13,"err writing to pipe from child");
			}
			close(pipefd[1]);
			exit(0);
		}
	}

    close(pipefd[1]);
	uint16_t xor;
	for(int i = 0; i < size; i++){
		if (read(pipefd[0],&xor,sizeof(xor)) != sizeof(xor)){
			err(13,"err reading from pipe from parent");
		}
		res^=xor;
	}
	
	close(pipefd[0]);
	close(fd);

    char res_str[8];
    snprintf(res_str, sizeof(res_str), "%04X\n", res);
    if (write(1, res_str, sizeof(res_str)) < 0) {
        err(14, "err writing result to stdout");
    }
    //or alternatively
    //dprintf(1,"%04X\n", res);

    for (int i = 0; i < size; i++){
        waitProcess();  
    }
    return 0;
}