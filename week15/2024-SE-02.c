#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <err.h>

const int MAX_PROGS=10;
char *progs[MAX_PROGS]; //not really needed, but for clarity
pid_t pids[MAX_PROGS];
int N; // defining here in order to use it in multiple functions without passing it around

void spawn(int i) {
    pid_t pid = fork();
    if (pid < 0) {
        err(2, "err fork");
    }
    if (pid == 0) {
        execl(progs[i], progs[i], (char *)NULL);
        err(3, "err execl");
    }
    pids[i] = pid;
}

int find_index_by_pid(pid_t pid) {
    for (int i = 0; i < N; i++) {
        if (pids[i] == pid)
            return i;
    }
    return -1;
}

int done() {
    for (int i = 0; i < N; i++) {
        if (pids[i] != 0)
            return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2 || argc > MAX_PROGS + 1) {
        errx(1, "1 to 10 programs needed as arguments");
    }

    N = argc - 1;
    for (int i = 0; i < N; i++) {
        progs[i] = argv[i + 1];
        spawn(i);
    }

    int status;
    pid_t pid;
    while ((pid = wait(&status)) > 0) {
        int i = find_index_by_pid(pid);
        if (i == -1)
            continue;

        if (WIFSIGNALED(status)) {
            for (int j = 0; j < N; j++) {
                if (j != i && pids[j] != 0) {
                    kill(pids[j], SIGTERM);
                }
            }
            for (int j = 0; j < N; j++) {
                if (j != i && pids[j] != 0) {
                    waitpid(pids[j], NULL, 0);
                }
            }
            exit(i + 1);
        } else if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                pids[i] = 0; // mark as done
                if (done()) {
                    exit(0);
                }
            } else {
                spawn(i);
            }
        }
    }

    if (errno == ECHILD) {
        exit(0);
    }

    err(5, "waitpid failed");
}
