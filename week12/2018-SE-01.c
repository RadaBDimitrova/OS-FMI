#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
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

int main(int argc,char* argv[]){

	if(argc!=2){
		errx(1,"1 arg needed");
	}
	int findfd[2];
	if(pipe(findfd) < 0){
		err(2,"err pipe");
	}
	int pid = fork();
	if (pid <0){
		err(3, "fork");
	}
	
	if(pid == 0){
		close(findfd[0]);
		dup2(findfd[1],1);
		close(findfd[1]);

		if(execlp("find","find", argv[1], "-type","f","-printf","%T@ %f\n",(char*)NULL) < 0){
			err(4,"err exec");
		}
        // could be execlp and on the next line err, but this is valid too
	}

	close(findfd[1]);

	int sortfd[2];
	if(pipe(sortfd) < 0){
		err(2,"err pipe");
	}
	
	pid = fork();
	if (pid < 0){
		err(3, "fork");
	}
	
	if(pid == 0){
		close(sortfd[0]); // close read end
		dup2(findfd[0],0); // read from find
        close(findfd[0]); // close read end for find
		dup2(sortfd[1],1); // write to sort
		close(sortfd[1]); // close write end for sort

		if(execlp("sort","sort", "-nr",(char*)NULL) < 0){
			err(4,"err exec");
		}
	}

	close(findfd[0]);
	close(sortfd[1]);

	int headfd[2];
	if(pipe(headfd) < 0){
		err(2,"err pipe");
	}
	
	pid = fork();
	if (pid < 0){
		err(3, "fork");
	}
	
	if(pid == 0){
		close(headfd[0]);
		dup2(sortfd[0],0);
        close(sortfd[0]);
		dup2(headfd[1],1);
		close(headfd[1]);

		if(execlp("head","head", "-n1",(char*)NULL) < 0){
			err(4,"err exec");
		}
	}

    close(sortfd[0]);
    close(headfd[1]);

    pid = fork();
    if (pid < 0) err(3, "fork error");
    if (pid == 0) {
        dup2(headfd[0], STDIN_FILENO);
        close(headfd[0]);

        execlp("cut", "cut", "-d", " ", "-f2-", (char*)NULL);
        err(4, "exec cut");
    }
    close(headfd[0]);
    
    for(int i = 0; i < 4; i++){
        waitProcess();
    }
    return 0;     
}