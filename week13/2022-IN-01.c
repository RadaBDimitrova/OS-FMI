#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <stdio.h>

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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        errx(1, "Usage: %s N D", argv[0]);
    }

    int N = strtol(argv[1], NULL, 10);
    int D = strtol(argv[2], NULL, 10);

    int pcfd[2];
    int cpfd[2];

    if (pipe(pcfd) == -1 || pipe(cpfd) == -1) {
        err(2, "pipe error");
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(3, "fork error");
    }

    if (pid == 0) {
        close(pcfd[1]);
        close(cpfd[0]);

        char s;
        for (int i = 0; i < N; i++) {
            if (read(pcfd[0], &s, 1) != 1) {
                err(4, "child read error");
            }

            if (write(1, "DONG\n", 5) != 5) {
				err(4, "child write error");
			}

            if (write(cpfd[1], "s", 1) != 1) {
                err(5, "child write error");
            }
        }

        close(pcfd[0]);
        close(cpfd[1]);
        _exit(0);
    } else {
        close(pcfd[0]);
        close(cpfd[1]);

        char s;
        for (int i = 0; i < N; i++) {
            if (write(1, "DING ", 5) != 5) {
				err(6, "parent write error");
			}

            if (write(pcfd[1], "s", 1) != 1) {
                err(6, "parent write error");
            }

            if (read(cpfd[0], &s, 1) != 1) {
                err(7, "parent read error");
            }

            sleep(D);
        }

        close(pcfd[1]);
        close(cpfd[0]);
        waitProcess();
    }

    return 0;
}
