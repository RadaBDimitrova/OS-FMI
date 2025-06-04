#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>
#include <stdint.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        errx(1, "3 args needed");
    }

    int N = strtol(argv[2], NULL, 10);
    if (N < 0 || N > 255) {
        errx(2, "N must be between 0 and 2^8 - 1");
    }

    int randfd = open("/dev/random", O_RDONLY);
    if (randfd < 0) {
        err(3, "err opening /dev/random");
    }

    for (int i = 0; i < N; i++){
        uint16_t S;
        if (read(randfd, &S, sizeof(S)) != sizeof(S)) {
            err(4, "err reading from /dev/random");
        }
        if (S < 1) {
            continue;
        }

        uint8_t bytes[65535]; //~(uint16_t)0
        if (read(randfd, bytes, S) != S) {
            err(6, "err reading random bytes");
        }

        int pipefd[2];
        if (pipe(pipefd) < 0) {
            err(7, "err creating pipe");
        }

        int pid = fork();
        if (pid < 0) {
            err(8, "err forking");
        }

        if (pid == 0) {
            close(pipefd[1]);
            if (dup2(pipefd[0], 0) < 0) {
                err(9, "err during dup2");
            }
            close(pipefd[0]);

            int nullfd = open("/dev/null", O_WRONLY);
            if (nullfd < 0) {
                err(10, "err opening /dev/null");
            }
            if (dup2(nullfd, 1) < 0) {
                err(11, "err during dup2 to /dev/null for stdout");
            }
            if (dup2(nullfd, 2) < 0) {
                err(12, "err during dup2 to /dev/null for stderr");
            }
            close(nullfd);
            execl(argv[1], argv[1], (const char*) NULL);
            err(13, "child failed to execute process %s", argv[1]);
        }

        close(pipefd[0]);
        if (write(pipefd[1], bytes, S) != S) {
            err(14, "err writing to pipe");
        }
        close(pipefd[1]);

        int status;
        if (waitpid(pid, &status, 0) < 0) {
            err(15, "err waiting for child pid");
        }

        if (!WIFEXITED(status)) {
            int outfd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (outfd < 0) {
                err(16, "err opening output file");
            }
            if (write(outfd, bytes, S) != S) {
                err(17, "err writing to output file");
            }
            close(outfd);
            close(randfd);
            return 42;
        }
    }
    int outfd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (outfd < 0) {
        err(18, "err opening output file");
    }
    close(outfd);
    close(randfd);
    return 0;
}