#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

const char* str = "found it!";

int main(int argc, char* argv[]) {
	
    if (argc < 2) {
        errx(26, "at least 1 arg needed");
    }

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        err(26, "err during pipe");
    }

    int children[argc - 1];

    for (int i = 1; i < argc; i++) {
        int pid = fork();

        if (pid < 0) {
            err(26, "err forking");
        }

        if (pid == 0) {
            close(pipefd[0]);
            if (dup2(pipefd[1], 1) < 0) {
                err(26, "err during dup2");
            }

            close(pipefd[1]);
            execlp(argv[i], argv[i], (char*)NULL);
            err(26, "child failed to execute process %s", argv[i]);
        }
        children[i - 1] = pid;
    }

    close(pipefd[1]);

    char byte;
    size_t curr = 0;
    while (read(pipefd[0], &byte, 1) == 1) {
        if (byte == str[curr]) {
            curr++;
        } else {
            if (byte == str[0]) {
    			curr = 1;
			} else {
    			curr = 0;
			}
        }

        if (curr == strlen(str)) {
            for (int i = 0; i < argc - 1; i++) {
                kill(children[i], SIGTERM);
            }
            for (int i = 0; i < argc - 1; i++) {
                waitpid(children[i], NULL, 0);
            }
            close(pipefd[0]);
            exit(0);
        }
    }

    close(pipefd[0]);

    for (int i = 0; i < argc - 1; i++) {
        waitpid(children[i], NULL, 0);
    }

    exit(1);
}
