#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <err.h>

const char *L[] = {"tic ","tac ","toe\n"};

void extraRead(int from, int to, int limit){
	int curr;
	while(read(from,&curr,sizeof(curr)) == sizeof(curr)){
        //separated the logic into cases and steps, so that it could be
        //explained better, otherwise not the best code layout for the funciton 
		if (curr < limit) {
			curr++;
			if (write(to,&curr,sizeof(curr)) != sizeof(curr)){
				err(26,"err writing to next pipe");
			}
			if (write(1,L[(curr-1)%3],4) != 4){
				err(27,"err writing word");
			}
		}
		else {
			curr++;
			if (write(to,&curr,sizeof(curr)) != sizeof(curr)){
				err(27,"err last writing to next pipe");
			}
			close(from);
			close(to);
			exit(0);
		}
	}
}

int main(int argc, char* argv[]){
	if(argc != 3){
		errx(1,"NC and WC needed");
	}

	char* endptr;
	int NC = strtol(argv[1],&endptr,10);
	int WC = strtol(argv[2],&endptr,10);
    if (NC < 1 || NC > 7 || WC < 1 || WC > 35) {
        errx(1, "NC must be between 1 and 7, WC must be between 1 and 35");
    }

	int ctr = 0;
	int pipefds[8][2];
	for(int i = 0; i <= NC; i++){
		if (pipe(pipefds[i]) < 0){
			err(2,"err pipe");
		}
	}
	for(int i = 0; i < NC; i++){
		int pid = fork();
		if (pid < 0){
			err(3,"err fork");
		}
		if (pid == 0){
			for (int j = 0; j <= NC; j++){
				if (j == i){
					close(pipefds[j][1]);
				}
				else if (j == i + 1){
					close(pipefds[j][0]);
				}
				else {
					close(pipefds[j][0]);
					close(pipefds[j][1]);
				}
			}
			extraRead(pipefds[i][0],pipefds[i+1][1],WC);
			exit(0);
		}
	}

	for(int j = 0; j <= NC; j++){
		if (j == NC){
			close(pipefds[j][1]);
		}
		else if (j == 0){
			close(pipefds[0][0]);
		}
		else {
			close(pipefds[j][0]);
			close(pipefds[j][1]);
		}
	}
	if (write(pipefds[0][1],&ctr,sizeof(ctr)) != sizeof(ctr)){
        err(4,"err writing to first pipe");
    }
	extraRead(pipefds[NC][0],pipefds[0][1],WC);

}