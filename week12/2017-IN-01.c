#include <stdint.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){

	int catfd[2];
	if (pipe(catfd) < 0){
		err(1, "pipe");
	}
	int pid = fork();
	if(pid<0){
		err(2,"fork");
	}

	if(pid == 0){
		close(catfd[0]);
		dup2(catfd[1], 1);
		close(catfd[1]);
		
		if(execlp("cut", "cut", "-d:", "-f7","/etc/passwd", (char*)NULL) < 0){
			err(3,"exec");
		}
	}
	close(catfd[1]);
	
	int sortfd[2];
	if(pipe(sortfd)<0){
		err(1, "pipe");
	}

	pid = fork();
	if(pid<0){
		err(2,"fork");
	}

	if(pid == 0){
		close(sortfd[0]); // close read end
		dup2(catfd[0],0); // read from cut
		dup2(sortfd[1],1); // write to sort
		close(catfd[0]); // close read end for cut
		close(sortfd[1]); // close write end for sort
		if(execlp("sort", "sort",(char*)NULL) < 0){
			err(3,"exec");
		}
	}
	close(sortfd[1]);
	close(catfd[0]);

	int uniqfd[2];
	if(pipe(uniqfd)<0){
		err(1, "pipe");
	}

	pid = fork();
	if(pid<0){
		err(2,"fork");
	}

	if(pid == 0){
		close(uniqfd[0]);
		dup2(sortfd[0],0);
		close(sortfd[0]);
		dup2(uniqfd[1],1);
		close(uniqfd[1]);

		if(execlp("uniq", "uniq", "-c",(char*)NULL) < 0){
			err(3,"exec");
		}
	}
	
	close(uniqfd[1]);
	close(sortfd[0]);

    //what is the idea with this NULL:
	while(wait(NULL)>0); //if we don't want to check status

	dup2(uniqfd[0],0);
	close(uniqfd[0]);

	if(execlp("sort", "sort", "-n", (char*)NULL) < 0){
		err(3, "exec");
	}

    /*alternative:
    pid = fork();
    if (pid < 0) {
        err(2, "fork");
    }

    if (pid == 0) {
        dup2(uniqfd[0], 0);
        close(uniqfd[0]);
        execlp("sort", "sort", "-n", (char*)NULL);
        err(3, "exec");
    }

    close(uniqfd[0]);
    while (wait(NULL) > 0); // Wait for all 4 children
    */
}