#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>

const char* str = "found it!";

int main(int argc, char* argv[]) {

	if (argc < 2) {
		errx(26, "at least 1 arg needed");
	}

	int pipefd[2];
    if (pipe(pipefd) < 0) {
		err(26, "err during pipe");
	}


	for (int i = 1; i < argc; i++) {
		int proc = fork();
		if (proc < 0) {
			err(26, "err forking");
		}

		if (proc == 0) {
			close(pipefd[0]);
            if (dup2(pipefd[1], 1) < 0) {
                err(26, "err during dup2");
            }
			close(pipefd[1]);
            execlp(argv[i], argv[i], (const char*) NULL);
            err(26, "child failed to execute process %s", argv[i]);
		}

	}

	close(pipefd[1]);

	char byte;
	size_t curr = 0;
	while (read(pipefd[0], &byte, sizeof(char)) == sizeof(char)) {
		if (byte == str[curr]) {
			curr++;
		} else {
			curr = 0;	
		}

		if (curr == strlen(str)) {
			close(pipefd[0]);
            if (kill(0, SIGTERM) < 0) { //kills all processes in the group
                err(26, "failed to kill all children");
            }
			exit(0);
		}

	}
	close(pipefd[0]);

	exit(1);
}