#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

// for clarity of constants
#define MAX_REGS 32
#define MAX_RAM 512
#define FILENAME_SIZE 8
#define INSTR_SIZE 4

typedef struct {
    uint16_t ram_size;
    uint16_t reg_count;
    char filename[FILENAME_SIZE];
} processor;

typedef struct {
    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
} instruction;

void execute(const processor* info) {
    uint8_t regs[MAX_REGS] = {0};
    uint8_t ram[MAX_RAM] = {0};

    int fd = open(info->filename, O_RDWR);
    if (fd < 0) {
        err(2,"err open");
    }

    if (read(fd, regs, info->reg_count) != info->reg_count) {
        err(3, "err reading registers");
    }

    if (read(fd, ram, info->ram_size) != info->ram_size) {
        err(4, "err reading RAM");
    }

    off_t instr_start = lseek(fd, 0, SEEK_CUR);
    instruction instr;

    while (read(fd, &instr, INSTR_SIZE) == INSTR_SIZE) {
        switch (instr.opcode) {
            case 0:
                regs[instr.op1] = regs[instr.op2] & regs[instr.op3];
                break;
            case 1:
                regs[instr.op1] = regs[instr.op2] | regs[instr.op3];
                break;
            case 2:
                regs[instr.op1] = regs[instr.op2] + regs[instr.op3];
                break;
            case 3:
                regs[instr.op1] = regs[instr.op2] * regs[instr.op3];
                break;
            case 4:
                regs[instr.op1] = regs[instr.op2] ^ regs[instr.op3];
                break;
            case 5:
                if (write(STDOUT_FILENO, &regs[instr.op1], 1) != 1) {
                    err(4, "write err to stdout");
                }
                break;
            case 6:
                sleep(regs[instr.op1]);
                break;
            case 7:
                regs[instr.op1] = ram[regs[instr.op2]];
                break;
            case 8:
                ram[regs[instr.op2]] = regs[instr.op1];
                break;
            case 9:
                if (regs[instr.op1] != regs[instr.op2]) {
                    off_t offset = instr_start + instr.op3 * INSTR_SIZE;
                    if (lseek(fd, offset, SEEK_SET) == -1)
                        err(5, "lseek err");
                }
                break;
            case 10:
                regs[instr.op1] = instr.op2;
                break;
            case 11:
                ram[regs[instr.op1]] = instr.op2;
                break;
            default:
                errx(6, "invalid opcode");
        }
    }

    lseek(fd, 0, SEEK_SET);
    if (write(fd, regs, info->reg_count) != info->reg_count)
        err(7, "write err registers");

    if (write(fd, ram, info->ram_size) != info->ram_size)
        err(8, "write err RAM");

    close(fd);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        errx(1, "2 args needed");
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0) {
        err(1, "err open");
    }

    processor info;
    while (read(input_fd, &info, sizeof(info)) == sizeof(info)) {
        pid_t pid = fork();
        if (pid < 0) err(9, "err fork");

        if (pid == 0) {
            close(input_fd);
            execute(&info);
        }
    }

    close(input_fd);

    while (wait(NULL) > 0);

    return 0;
}
