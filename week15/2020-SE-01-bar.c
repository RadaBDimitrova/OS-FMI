//bar
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void waitProcess(void);

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


int main(int argc, char* argv[]){
	if(argc != 2){
		errx(1,"command needed");
	}
	
	int pid = fork();
	if( pid < 0){
		err(6,"fork");
	}
	
	if (pid == 0){
		int	fd = open("foobarpipe", O_RDONLY);
		if ( fd < 0){
			err(4,"err opening fifo");
		}

		if (dup2(fd,0) < 0){
			err(7,"err dup2");
		}
		
		if(execl(argv[1],argv[1],(char*)NULL) < 0){
			err(8, "exec");
		}
	}

	waitProcess();
	if(unlink("foobarpipe") < 0){
		err(10,"can't unlink");
	}
	
}