#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){

        const char prompt[] = "Enter command:\n";
        while(1) {

        if(write(1,prompt,strlen(prompt)) < 0){
                err(1, "err write");
        }

        char buff[4096];
        int length;
        if((length = read(0,&buff,sizeof(buff))) > 0){
                buff[length - 1] = '\0';
        }

        if(length == -1){
                err(10, "err reading command");
        }

        if(strcmp(buff,"exit") == 0){
        exit(0);
        }

        int pid = fork();
        if(pid < 0){
                err(2,"err fork");
        }

        if(pid == 0){
                if(execlp(comm, comm,(char*)NULL) < 0){
                        err(4,"err executing command");
                }
        }

    // could be separated in a function like: void asserted_wait(void) { ... }
        int status;
        if(wait(&status) < 0){
                err(5,"can't wait");
        }
        if(!WIFEXITED(status)){
                errx(6,"child not exited normally");
        }

        if(WEXITSTATUS(status) != 0){
                errx(7,"child status not 0");
        }

        }

}